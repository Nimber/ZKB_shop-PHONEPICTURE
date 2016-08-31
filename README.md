# ZKB_shop-PHONEPICTURE
This is an Application for SupCon Robot competition
## Compiling
To compiling this project, you should have Visual Studio 2015 and OpenCV 2.4.11(Recommend) in your target mechine. If you still cannot compiling this project, you can delete the following lines in ZKB_shop.vcproj in Notepad:
```xml
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="E:\opencv2.4\opencv2411.props" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="E:\opencv2.4\opencv2411.props" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="E:\opencv2.4\opencv2411.props" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
    <Import Project="E:\opencv2.4\opencv2411.props" />
  </ImportGroup>
  ```
and then add your OpenCV setup to the project in Visual Studio
## Resource
* [OpenCV 2.4.11](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.11/opencv-2.4.11.exe/download "Get Opencv 2.4.11")
* [Visual Studio 2015](https://beta.visualstudio.com/downloads/ "Get Visual Studio 2015")
* [About this projects](https://shengni.me/shopping-robot/ "About this project")
