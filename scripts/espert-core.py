#!/usr/bin/python3

###########################################################################
#                                                                         #
#    Build manager for espert-core                                        #
#                                                                         #
#    Allows easy configuring, building and testing of espert-core.        #
#    Run python3 espert-core.py --help for further instructions.          #
#                                                                         #
###########################################################################

from argparse import ArgumentParser, Namespace
from common import EnumAction, ROOT, BUILD_DIR
from enum import Enum
import multiprocessing
import os
from shutil import rmtree
import subprocess
import sys

GET_WSI_COMMAND = "loginctl show-session $(loginctl | grep $(whoami) | awk '{print $1}') -p Type | cut -d'=' -f2"
CTEST_FILE = "CTestTestfile.cmake"


class WSI(Enum):
    XCM = "xcm"
    XLIB = "xlib"
    WAYLAND = "wayland"
    D2D = "d2d"


class BuildType(Enum):
    DEBUG = "debug"
    RELEASE = "release"


class Compiler(Enum):
    GCC = "gcc"
    CLANG = "clang"


def get_cpu_count() -> int:
    return multiprocessing.cpu_count()


def get_number_of_jobs() -> int:
    return max(1, get_cpu_count() - 2)


def get_wsi_type() -> str:
    proc = subprocess.Popen(GET_WSI_COMMAND, shell=True, stdout=subprocess.PIPE)
    proc.wait()
    return proc.stdout.read().decode("utf-8").strip()


def get_configure_command(args: Namespace) -> str:
    CMD = "cmake -S . -B build"

    if args.build_type == BuildType.DEBUG:
        CMD += f" -DCMAKE_BUILD_TYPE=Debug"
    elif args.build_type == BuildType.RELEASE:
        CMD += f" -DCMAKE_BUILD_TYPE=Release"

    if args.compiler == Compiler.GCC:
        CMD += " -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
    elif args.compiler == Compiler.CLANG:
        CMD += " -DCMAKE_C_COMPILER=clang-17 -DCMAKE_CXX_COMPILER=clang++-17"

    if sys.platform.startswith("linux"):
        if args.wsi is None:
            wsi = get_wsi_type()
        else:
            wsi = args.wsi.value
        if wsi == "x11":
            wsi = "xlib"
        CMD += f" -DVKB_WSI_SELECTION={wsi.upper()}"

    if args.build_tests:
        CMD += f" -DESP_BUILD_TESTS=ON"

    if args.export_compile_commands:
        CMD += " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

    return CMD


def get_build_command(args) -> str:
    CMD = f"make -j{args.jobs}"

    return CMD


def run_command_detached(command: str, cwd: str) -> None:
    print(command)
    proc = subprocess.Popen(command, cwd=cwd, shell=True)
    proc.wait()
    if proc.returncode != 0:
        sys.exit(proc.returncode)


def run_configure(args: Namespace) -> None:
    if args.clean and os.path.exists(BUILD_DIR):
        rmtree(BUILD_DIR)

    if not args.clean and not os.path.exists(BUILD_DIR):
        os.mkdir(BUILD_DIR)

    configure_command = get_configure_command(args)
    run_command_detached(configure_command, ROOT)


def run_build(args: Namespace) -> None:
    if args.clean or not os.path.exists(BUILD_DIR) or os.listdir(BUILD_DIR) == 0:
        run_configure(args)

    build_command = get_build_command(args)
    run_command_detached(build_command, BUILD_DIR)


def run_tests(args: Namespace) -> None:
    if args.clean or not os.path.exists(BUILD_DIR / CTEST_FILE):
        args.build_tests = True
        run_build(args)

    test_command = "ctest"
    run_command_detached(test_command, BUILD_DIR)


def get_parser() -> ArgumentParser:
    main_parser = ArgumentParser(
        prog="espert-core", description="Manage espert-core project."
    )

    subparsers = main_parser.add_subparsers(dest="cmd")
    configure_parser = subparsers.add_parser(
        "configure", help="Configure project with cmake."
    )
    configure_parser.set_defaults(func=run_configure)
    build_parser = subparsers.add_parser("build", help="Build project.")
    build_parser.set_defaults(func=run_build)
    test_parser = subparsers.add_parser("test", help="Run tests.")
    test_parser.set_defaults(func=run_tests)

    build_type_group = main_parser.add_mutually_exclusive_group()
    build_type_group.add_argument(
        "-d",
        "--debug",
        action="store_const",
        dest="build_type",
        const=BuildType.DEBUG,
        help="Use debug configuration. (default)",
    )
    build_type_group.add_argument(
        "-r",
        "--release",
        action="store_const",
        dest="build_type",
        const=BuildType.RELEASE,
        help="Use release configuration.",
    )
    main_parser.set_defaults(build_type=BuildType.DEBUG)

    compiler_selection_group = main_parser.add_mutually_exclusive_group()
    compiler_selection_group.add_argument(
        "--gcc",
        action="store_const",
        dest="compiler",
        const=Compiler.GCC,
        help="Pick gcc and g++ as project compilers. (default)",
    )
    compiler_selection_group.add_argument(
        "--clang",
        action="store_const",
        dest="compiler",
        const=Compiler.CLANG,
        help="Pick clang and clang++ as project compilers.",
    )

    main_parser.add_argument(
        "-c",
        "--clean",
        required=False,
        default=False,
        action="store_true",
        help="Clean out build directory first.",
    )
    main_parser.add_argument(
        "-e",
        "--export-compile-commands",
        required=False,
        default=False,
        action="store_true",
        help="Add CMAKE_EXPORT_COMPILE_COMMANDS to build.",
    )
    main_parser.add_argument(
        "-t",
        "--build-tests",
        required=False,
        default=False,
        action="store_true",
        help="Build tests for project.",
    )
    main_parser.add_argument(
        "-j",
        "--jobs",
        required=False,
        type=int,
        default=get_number_of_jobs(),
        help="Number of paralell jobs during build. (default: max system threads - 2)",
    )
    main_parser.add_argument(
        "-w",
        "--wsi",
        required=False,
        type=WSI,
        action=EnumAction,
        help="Select WSI type for linux systems.",
    )

    return main_parser


if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()
    if args.cmd is None:
        print("Command not given.")
        parser.print_usage()
        sys.exit(1)
    args.func(args)
