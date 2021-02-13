set(CPACK_GENERATOR "DragNDrop") # DMG

# https://cmake.org/cmake/help/latest/module/CPack.html
# https://cmake.org/cmake/help/latest/cpack_gen/dmg.html#cpack_gen:CPack%20DragNDrop%20Generator

set(CPACK_RPM_PACKAGE_NAME "cmdr11-devel")
set(CPACK_RPM_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})

#set(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION})

#Generators
#7Z                           = 7-Zip file format
#Bundle                       = Mac OSX bundle
#DragNDrop                    = Mac OSX Drag And Drop
#External                     = CPack External packages
#IFW                          = Qt Installer Framework
#NSIS                         = Null Soft Installer
#NSIS64                       = Null Soft Installer (64-bit)
#NuGet                        = NuGet packages
#OSXX11                       = Mac OSX X11 bundle
#PackageMaker                 = Mac OSX Package Maker installer
#STGZ                         = Self extracting Tar GZip compression
#TBZ2                         = Tar BZip2 compression
#TGZ                          = Tar GZip compression
#TXZ                          = Tar XZ compression
#TZ                           = Tar Compress compression
#TZST                         = Tar Zstandard compression
#ZIP                          = ZIP file format
#productbuild                 = Mac OSX pkg