# CMakeList.txt: SimpleCompiler 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("SimpleCompiler")

include_directories(./include)              #头文件位置
aux_source_directory(./src DIR_TOOT_SRCS)
aux_source_directory(./src/Lexer DIR_TOOT_SRCS)
aux_source_directory(./src/Parser DIR_TOOT_SRCS)
aux_source_directory(./src/Semantic DIR_TOOT_SRCS)
aux_source_directory(./src/Output DIR_TOOT_SRCS)
aux_source_directory(./src/Optimization DIR_TOOT_SRCS)

# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME} ${DIR_TOOT_SRCS})

# 将相关的语法规则、语义规则等复制到工作目录
file(GLOB RESOURCES "./info/*.txt")
add_custom_command(TARGET SimpleCompiler
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${RESOURCES}
			$<TARGET_FILE_DIR:SimpleCompiler>)

# TODO: 如有需要，请添加测试并安装目标。
