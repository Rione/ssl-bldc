Import("env")

#
# Dump build environment (for debug)
print(env.Dump())
#

flags = [
    "-Os",  # optimize for size
    "-ffunction-sections",  # place each function in its own section
    "-fdata-sections",
    "-Wall",
    "-mthumb",
    "-mfloat-abi=hard",
    "-mfpu=fpv4-sp-d16",
    "-mcpu=%s" % env.BoardConfig().get("build.cpu"),
    "-nostdlib"
]

env.Append(CCFLAGS=flags, LINKFLAGS=flags)
