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
from common import (
    EnumAction,
    ROOT,
    BUILD_DIR,
    LIB_DIR,
    BuildType,
    run_command,
    get_wsi_type,
    WSI,
    get_optimal_job_count,
    get_lib_names,
    is_platform_linux,
    is_platform_windows,
    CmakeConfigureCommand,
    CmakeBuildCommand,
    CmakeParameter,
    Compiler,
)
from enum import Enum
import os
from shutil import rmtree
import sys

CTEST_FILE = "CTestTestfile.cmake"
VK_LAYER_PATH = BUILD_DIR / "validation_layers"


def get_configure_command(args: Namespace) -> str:
    CMD = CmakeConfigureCommand(build_dir="build", source_dir=".")
    CMD.add_parameter(CmakeParameter("CMAKE_BUILD_TYPE", args.build_type.value))
    CMD.add_parameter(CmakeParameter("CMAKE_EXPORT_COMPILE_COMMANDS", "ON"))

    if args.compiler == Compiler.GCC:
        CMD.add_parameter(CmakeParameter("CMAKE_C_COMPILER", "gcc"))
        CMD.add_parameter(CmakeParameter("CMAKE_CXX_COMPILER", "g++"))
    elif args.compiler == Compiler.CLANG:
        CMD.add_parameter(CmakeParameter("CMAKE_C_COMPILER", "clang-17"))
        CMD.add_parameter(CmakeParameter("CMAKE_CXX_COMPILER", "clang++-17"))

    if is_platform_linux():
        CMD.add_parameter(CmakeParameter("VKB_WSI_SELECTION", args.wsi.value.upper()))

    if args.build_tests:
        CMD.add_parameter(CmakeParameter("ESP_BUILD_TESTS", "ON"))

    if args.build_docs:
        CMD.add_parameter(CmakeParameter("ESP_BUILD_DOCS", "ON"))

    if args.vvl:
        CMD.add_parameter(CmakeParameter("ESP_BUILD_VVL", "ON"))

    return str(CMD)


def get_build_command(args: Namespace) -> str:
    CMD = CmakeBuildCommand(build_dir="build", jobs=args.jobs)

    return str(CMD)


def clean_libs(args: Namespace):
    for libname in get_lib_names():
        libpath = LIB_DIR / libname / args.build_type.value
        if os.path.exists(libpath):
            rmtree(libpath)


def run_configure(args: Namespace) -> None:
    if args.hard_clean:
        clean_libs(args)

    if args.clean and os.path.exists(BUILD_DIR):
        rmtree(BUILD_DIR)

    if not args.clean and not os.path.exists(BUILD_DIR):
        os.mkdir(BUILD_DIR)

    configure_command = get_configure_command(args)
    run_command(configure_command, ROOT)


def run_build(args: Namespace) -> None:
    if args.clean or not os.path.exists(BUILD_DIR) or os.listdir(BUILD_DIR) == 0:
        run_configure(args)

    build_command = get_build_command(args)
    run_command(build_command, ROOT)


def run_tests(args: Namespace) -> None:
    if args.clean or not os.path.exists(BUILD_DIR / CTEST_FILE):
        args.build_tests = True
        run_build(args)

    my_env = os.environ.copy()
    if args.build_type == BuildType.DEBUG and args.vvl:
        my_env["LD_LIBRARY_PATH"] = str(VK_LAYER_PATH)
        my_env["VK_LAYER_PATH"] = str(VK_LAYER_PATH)

    CMD = "ctest"
    run_command(CMD, BUILD_DIR, my_env)


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
        "-a",
        "--hard-clean",
        required=False,
        default=False,
        action="store_true",
        help="Clean out build directory and cached libs.",
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
        "-x",
        "--build-docs",
        required=False,
        default=False,
        action="store_true",
        help="Build docs for project.",
    )
    main_parser.add_argument(
        "-j",
        "--jobs",
        required=False,
        type=int,
        default=get_optimal_job_count(),
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
    main_parser.add_argument(
        "--vvl",
        required=False,
        default=False,
        action="store_true",
        help="Build vulkan validation layers.",
    )

    return main_parser


if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()

    if args.hard_clean:
        args.clean = True

    if args.wsi is None and is_platform_linux():
        args.wsi = get_wsi_type()

    if args.cmd is None:
        print("Command not given.")
        parser.print_usage()
        sys.exit(1)

    args.func(args)
