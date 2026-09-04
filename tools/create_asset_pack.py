#!/usr/bin/env python3
"""Create a Grid++ SQLite asset pack from named PNG files."""

import argparse
import sqlite3
import struct
import zlib
from pathlib import Path

def parse_asset(value: str) -> tuple[str, Path]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("asset must use NAME=PNG_PATH")
    name, raw_path = value.split("=", 1)
    if not name:
        raise argparse.ArgumentTypeError("asset name cannot be empty")
    path = Path(raw_path)
    if not path.is_file():
        raise argparse.ArgumentTypeError(f"image does not exist: {path}")
    return name, path


def paeth(left: int, above: int, upper_left: int) -> int:
    estimate = left + above - upper_left
    left_distance = abs(estimate - left)
    above_distance = abs(estimate - above)
    upper_left_distance = abs(estimate - upper_left)
    if left_distance <= above_distance and left_distance <= upper_left_distance:
        return left
    if above_distance <= upper_left_distance:
        return above
    return upper_left


def read_png_rgba(path: Path) -> bytes:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{path} is not a PNG file")

    width = height = bit_depth = color_type = interlace = None
    compressed = bytearray()
    offset = 8
    while offset < len(data):
        if offset + 12 > len(data):
            raise ValueError(f"{path} contains a truncated PNG chunk")
        length = struct.unpack(">I", data[offset : offset + 4])[0]
        chunk_type = data[offset + 4 : offset + 8]
        chunk_data = data[offset + 8 : offset + 8 + length]
        if len(chunk_data) != length:
            raise ValueError(f"{path} contains a truncated PNG chunk")
        offset += 12 + length

        if chunk_type == b"IHDR":
            width, height, bit_depth, color_type, _, _, interlace = struct.unpack(
                ">IIBBBBB", chunk_data
            )
        elif chunk_type == b"IDAT":
            compressed.extend(chunk_data)
        elif chunk_type == b"IEND":
            break

    if (width, height) != (32, 32):
        raise ValueError(f"{path} is {width}x{height}; expected 32x32")
    if bit_depth != 8 or color_type not in (2, 6) or interlace != 0:
        raise ValueError(
            f"{path} must be a non-interlaced 8-bit RGB or RGBA PNG"
        )

    channels = 4 if color_type == 6 else 3
    stride = width * channels
    raw = zlib.decompress(bytes(compressed))
    if len(raw) != height * (stride + 1):
        raise ValueError(f"{path} contains unexpected pixel data")

    rows: list[bytearray] = []
    cursor = 0
    for _ in range(height):
        filter_type = raw[cursor]
        cursor += 1
        encoded = raw[cursor : cursor + stride]
        cursor += stride
        row = bytearray(stride)
        previous = rows[-1] if rows else bytearray(stride)
        for index, value in enumerate(encoded):
            left = row[index - channels] if index >= channels else 0
            above = previous[index]
            upper_left = previous[index - channels] if index >= channels else 0
            if filter_type == 0:
                predictor = 0
            elif filter_type == 1:
                predictor = left
            elif filter_type == 2:
                predictor = above
            elif filter_type == 3:
                predictor = (left + above) // 2
            elif filter_type == 4:
                predictor = paeth(left, above, upper_left)
            else:
                raise ValueError(f"{path} uses unsupported PNG filter {filter_type}")
            row[index] = (value + predictor) & 0xFF
        rows.append(row)

    if channels == 4:
        return b"".join(rows)

    rgba = bytearray()
    for row in rows:
        for index in range(0, len(row), 3):
            rgba.extend(row[index : index + 3])
            rgba.append(255)
    return bytes(rgba)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Create a Grid++ asset pack from 32x32 PNG images."
    )
    parser.add_argument("output", type=Path, help="output .db path")
    parser.add_argument(
        "assets",
        metavar="NAME=PNG_PATH",
        type=parse_asset,
        nargs="+",
        help="asset name and source PNG",
    )
    args = parser.parse_args()

    names = [name for name, _ in args.assets]
    if len(names) != len(set(names)):
        parser.error("asset names must be unique")

    with sqlite3.connect(args.output) as db:
        db.execute("DROP TABLE IF EXISTS sprites")
        db.execute(
            """
            CREATE TABLE sprites (
                id INTEGER PRIMARY KEY,
                name TEXT,
                tags TEXT,
                image_data BLOB
            )
            """
        )
        for name, path in args.assets:
            image_bytes = read_png_rgba(path)
            db.execute(
                "INSERT INTO sprites (name, tags, image_data) VALUES (?, ?, ?)",
                (name, "", image_bytes),
            )

    print(f"Wrote {len(args.assets)} assets to {args.output}")


if __name__ == "__main__":
    main()
