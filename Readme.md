Point and Click Hentai Game.
Stopped working on it around the start of 2017.
Will be revised and released for free with pushed donation.
Uses Visual Studio 15.

## VS15 info

Property Pages -> VC++ Directories -> Include Directories =<br/> $(VC_IncludePath);$(WindowsSDK_IncludePath);$(SolutionDir)deps/include;$(IncludePath)
Property Pages -> VC++ Directories -> Library Directories =<br/> $(SolutionDir)deps/lib;$(LibraryPath)$(VC_LibraryPath_x86);$(WindowsSDK_LibraryPath_x86);$(NETFXKitsDir)Lib\um\x86
Property Pages -> C/C++ -> General ->  Additional Include Directories =<br/> $(SolutionDir)deps/boost_1_63_0
Property Pages -> Linker -> General -> Additional Library Directories =<br/> $(SolutionDir)deps/boost_1_63_0\stage\lib
Property Pages -> Linker -> Input -> Additional Dependencies =<br/> 

Make sure you're on the right mode(release, debug etc.)!
