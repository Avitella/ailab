env = Environment()

env.Append(CXXFLAGS = ["-std=c++0x", "-O3"])
env.Append(CXXFLAGS = ["-Wall", "-W", "-g"])

env.Append(CPPPATH = [".", "./src"])
env.Append(LINKFLAGS = ["-g"])

console_tool = env.Program("./bin/console-tool",
    ["./src/console_tool.cc"],
    LIBS = ["mysqlclient"]
)
