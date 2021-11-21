set(CPACK_IFW_ROOT "/home/jlemein/Qt/QtIFW-4.1.1/")
set(CPACK_IFW_VERBOSE ON)
set(CPACK_GENERATOR "IFW;ZIP")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_PACKAGE_ICON "sun.png")
set(CPACK_PACKAGE_VENDOR "Lemon Software")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package-installers")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/LICENSE.md)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/README.md)
include(CPack)
include(CPackIFW)

cpack_add_component(examples
        DISPLAY_NAME "Examples"
        DESCRIPTION "Example portfolio to show capabilities"
        DOWNLOADED)
cpack_add_component(renderer
        DISPLAY_NAME "Izzy renderer"
        DESCRIPTION "Izzy Renderer"
        DOWNLOADED)