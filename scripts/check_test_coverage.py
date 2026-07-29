#!/usr/bin/env python3
"""Enforce the convention: src/foo.cpp -> tests/foo_test.cpp."""

from __future__ import annotations

import argparse
from pathlib import Path
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--src-dir", required=True, type=Path)
    parser.add_argument("--tests-dir", required=True, type=Path)
    parser.add_argument("--exclude", action="append", default=[])
    args = parser.parse_args()

    excluded = set(args.exclude)
    source_files = sorted(
        path for path in args.src_dir.rglob("*.cpp")
        if path.name not in excluded
    )

    missing = [
        args.tests_dir / f"{source.stem}_test.cpp"
        for source in source_files
        if not (args.tests_dir / f"{source.stem}_test.cpp").is_file()
    ]

    if missing:
        print("Missing required test files:", file=sys.stderr)
        for test_file in missing:
            print(f"  - {test_file}", file=sys.stderr)
        print(
            "\nConvention: every non-excluded src/foo.cpp needs "
            "tests/foo_test.cpp.",
            file=sys.stderr,
        )
        return 1

    print(f"Test-file coverage passed: {len(source_files)} source files checked.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
