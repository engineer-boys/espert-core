#!/usr/bin/python3
from argparse import ArgumentParser, Namespace
from common import (
    LIB_DIR,
    EXTERNAL_DIR,
    WSI,
    EnumAction,
    BuildType,
    run_command,
    CmakeBuildCommand,
    CmakeConfigureCommand,
    CmakeInstallCommand,
    CmakeParameter,
    get_optimal_job_count,
    get_lib_names,
    get_wsi_type,
)
from libconf import ESPERT_LIBS_CONF
import os
from shutil import rmtree

CMAKE_LISTS_FILENAME = "CMakeLists.txt"


def update_wsi_params(args: Namespace) -> None:
    ESPERT_LIBS_CONF["vulkan"].append(
        CmakeParameter("VKB_WSI_SELECTION", args.wsi.value.upper())
    )
    ESPERT_LIBS_CONF["volk"].append(
        CmakeParameter("VKB_WSI_SELECTION", args.wsi.value.upper())
    )
    if args.wsi == WSI.WAYLAND:
        ESPERT_LIBS_CONF["glfw"].append(CmakeParameter("GLFW_USE_WAYLAND", "ON"))


def configure_library(args: Namespace) -> None:
    if not os.path.exists(LIB_DIR / args.name):
        os.mkdir(LIB_DIR / args.name)
    if os.path.exists(LIB_DIR / args.name / CMAKE_LISTS_FILENAME):
        source_dir = "."
    else:
        source_dir = EXTERNAL_DIR / args.name
    CMD = CmakeConfigureCommand(build_dir="build", source_dir=source_dir)

    CMD.add_parameter(CmakeParameter("CMAKE_BUILD_TYPE", args.build_type.value))

    for param in ESPERT_LIBS_CONF[args.name]:
        CMD.add_parameter(param)

    run_command(str(CMD), LIB_DIR / args.name)


def build_library(args: Namespace) -> None:
    CMD = CmakeBuildCommand(build_dir="build", jobs=args.jobs)
    run_command(str(CMD), LIB_DIR / args.name)


def install_library(args: Namespace) -> None:
    prefix = LIB_DIR / args.name / args.build_type.value

    CMD = CmakeInstallCommand(install_dir="build", prefix=prefix)

    run_command(str(CMD), LIB_DIR / args.name)


def cleanup(args: Namespace) -> None:
    rmtree(LIB_DIR / args.name / "build")


def get_parsed_args() -> Namespace:
    parser = ArgumentParser(description="Build and install library.")
    parser.add_argument(
        "name",
        type=str,
        choices=get_lib_names(),
        help="Name of library to install.",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        required=False,
        type=int,
        default=get_optimal_job_count(),
        help="Number of paralell jobs during build. (default: max system threads - 2)",
    )
    build_type_group = parser.add_mutually_exclusive_group()
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
    parser.set_defaults(build_type=BuildType.DEBUG)
    parser.add_argument(
        "-w",
        "--wsi",
        required=False,
        type=WSI,
        action=EnumAction,
        help="Select WSI type for linux systems.",
    )

    return parser.parse_args()


if __name__ == "__main__":
    args = get_parsed_args()

    if args.wsi is None:
        args.wsi = get_wsi_type()

    update_wsi_params(args)

    configure_library(args)

    build_library(args)

    install_library(args)

    cleanup(args)
