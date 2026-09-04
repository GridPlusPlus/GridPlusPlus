import importlib.util
import sqlite3
import struct
import subprocess
import sys
import tempfile
import unittest
import zlib
from pathlib import Path


SCRIPT_PATH = Path(__file__).parents[1] / "tools" / "create_asset_pack.py"
SPEC = importlib.util.spec_from_file_location("create_asset_pack", SCRIPT_PATH)
assert SPEC is not None and SPEC.loader is not None
create_asset_pack = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(create_asset_pack)


def chunk(kind: bytes, data: bytes) -> bytes:
    checksum = zlib.crc32(kind + data) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", checksum)


def rgba_png(width: int, height: int, pixel: bytes) -> bytes:
    rows = b"".join(b"\x00" + pixel * width for _ in range(height))
    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    return (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", header)
        + chunk(b"IDAT", zlib.compress(rows))
        + chunk(b"IEND", b"")
    )


class AssetPackToolTest(unittest.TestCase):
    def test_decodes_32_by_32_rgba_png(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "player.png"
            path.write_bytes(rgba_png(32, 32, bytes((10, 20, 30, 40))))

            result = create_asset_pack.read_png_rgba(path)

            self.assertEqual(len(result), 4096)
            self.assertEqual(result[:4], bytes((10, 20, 30, 40)))

    def test_rejects_wrong_dimensions(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "wide.png"
            path.write_bytes(rgba_png(64, 32, bytes((10, 20, 30, 255))))

            with self.assertRaisesRegex(ValueError, "expected 32x32"):
                create_asset_pack.read_png_rgba(path)

    def test_command_creates_loadable_asset_row(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "player.png"
            output = Path(directory) / "assets.db"
            source.write_bytes(rgba_png(32, 32, bytes((10, 20, 30, 40))))

            subprocess.run(
                [sys.executable, str(SCRIPT_PATH), str(output), f"player={source}"],
                check=True,
                capture_output=True,
                text=True,
            )

            with sqlite3.connect(output) as database:
                row = database.execute(
                    "SELECT name, length(image_data) FROM sprites"
                ).fetchone()
            self.assertEqual(row, ("player", 4096))


if __name__ == "__main__":
    unittest.main()
