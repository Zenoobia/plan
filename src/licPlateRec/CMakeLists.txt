#File(GLOB licPlateRec_SRC "${PROJECT_SOURCE_DIR}/src/licPlateRec/*.cpp" "${PROJECT_SOURCE_DIR}/src/licPlateRec/*.cxx" "${PROJECT_SOURCE_DIR}/src/licPlateRec/*.hpp" "${PROJECT_SOURCE_DIR}/src/licPlateRec/*.inl")
#FILE(GLOB licPlateRec_SRC "*.cpp" "*.cxx" "*.hpp" "*.inl")
#set(licPlateRec_SRC "src/licPlateRec/*.cpp")

set(CUR_DIR ${PROJECT_SOURCE_DIR}/licPlateRec)
FILE(GLOB licPlateRec_SRC "${PROJECT_SOURCE_DIR}/licPlateRec/*.cpp" "${PROJECT_SOURCE_DIR}*.cxx" "${PROJECT_SOURCE_DIR}*.hpp" "${PROJECT_SOURCE_DIR}*.inl")

if(WIN32) # if windows and not linux
else()
	LIST(REMOVE_ITEM licPlateRec_SRC
    ${CUR_DIR}/MainForm.cpp
    ${CUR_DIR}/TrainSVM.cpp
    ${CUR_DIR}/jackylib.cpp
    ${CUR_DIR}/licCore.cpp
    )
endif()

