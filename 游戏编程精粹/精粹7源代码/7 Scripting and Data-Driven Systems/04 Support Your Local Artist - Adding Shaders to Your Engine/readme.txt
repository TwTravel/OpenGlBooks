Game Programming Gems 7 
Support Your Local Artist - Adding Shaders To Your Engine
Author: Curtiss Murphy

*******************
INTRODUCTION
*******************

This text file goes along with the Game Programming Gems 7 chapter described above. The source material found on this CD for this chapter was taken from the Delta3D Open Source Game engine. The materials here are just a snippet of the full project and source material. The original project is the Delta3D Tank Target Tutorial and can be pulled down as part of the Delta3D engine from http://www.delta3d.org.  

To run this application, simply run GameStart.bat in the BlimpShaderTutorial. To experiment with loading shaders at runtime, make whatever changes you want to the shaders found in ./BlimpShaderTutorial/StageProject/Shaders and press 'P'. If you do not already have Visual Studio 8 installed, you may need to run 'vcredist_x86.exe' to install the msvc8 dlls. 

*******************
HELP COMMANDS
*******************
 
This is a simple demo app which unfortunately means there is no in game help for keyboard commands. The following commands can be used in game: 

<space> - Start/stop the tank's engine (so you can move)
<I,J,K,L> - Turn & Move the tank (once engine is started)
<W,A,S,D> - Turn & Move the camera (independent of the tank)
<U,O> - Rotate turret
<F> - Damage a blimp (if one is targeted)
<R> - Reset everything
<P> - Reloads and reapplies all the shaders (for artist testing)
<Enter> - Give the tank a little speed boost
<Insert> - Toggle drawing statistics

*******************
RECORD AND PLAYBACK
*******************
 
This application is also capable of doing a full record and playback of all runtime behaviors. A full explanation of this is outside the scope of this chapter but may help some developers test various behaviors. Basically, you have to run the app once as normal and begin a recording by pressing '2'. Once you are finished, return to IDLE with '1' and exit. Then, add '--startPlayback 1 --mapName mapone_playback.xml' to your command line args and restart the app. Alternately, you can just run 'GameStartWithPlayback.bat'. For more information on record and playback, see the Delta3D website tutorials (http://www.delta3d.org).

The following are record and playback commands:
<1> - Return to 'IDLE' mode from Record or Playback
<2> - Begin 'RECORD'. All messages are recorded to the files, "D3DDefaultMessageLog.dli" and "D3DDefaultMessageLog.dlm".
<3> - Restart 'PLAYBACK'. This can only be done if you used --startPlayback 1 and specified a --mapName. 
<6> - Pause game play. Only available in PLAYBACK.
<7> - Slow down. Only available in PLAYBACK. 
<8> - Speed Up. Only available in PLAYBACK.

