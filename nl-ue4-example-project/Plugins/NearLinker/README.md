# Quick start

1. Open a project
2. Go to project settings and enable the plugin
3. Restart the Unreal Editor
4. Go to project settings > Plugins > Nearlinker
5. Set the "Integration server" "Executable path" to the location of node.exe
6. Set the "Integration server" "Working directory" to the location of the nl-api-server directory from the NearLinker git repository
7. If the integration server runs locally, create a "Certificates" folder in your project's "Content" diractory, and copy the integration server CA to "Content/Certificates/cacert.pem" 
7. Use the plugin in blueprints. See BP_NearMenu for an example of how to use it.


#Source code

In the Source directory, you will find the following files

## Settings

This adds a "Nearlinker" section in the project settings. In particular, it allows you to set the integration server url from the editor.

## WalletComponent

This defines an ActorComponent which can load, store, and save credentials for modifying requests to the integration server. It can also request creation of new credentials and save them.

## FunctionLibrary

This defines functions to interact with the integration server.

