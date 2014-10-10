As mentioned, we've set up a VirtualBox image of a (pared-down) Linux with the right tools to build Yalnix.

Here the instructions:

0. install VirtualBox on your machine. (See  https://www.virtualbox.org/wiki/Downloads)

1. Download the VirtualBox appliance file at /net/class/cs58/path/yalnix/vbox/CS58YalnixAdditions.ova     

2. Import it into VirtualBox by selecting File -> Import Appliance and selecting the file. Proceed through the steps; defaults should work fine.

3. Set up a shared folder seen by both your host machine and the VBox.
   - Create the directory on your host
   - Edit the settings for the virtual machine and go to "Shared Folders". Add a folder on your local machine that you want to share.
   - select the auto-mount option

 In the virtual machine, browse to /media/. Your shared folder should appear there.

4. Boot your machine  The accounts are root/cs58root and cs58/cs58yalnix;  the network is shut down, for security reasons.

5. A copy of the relevant yalnix files lives in /yalnix.          In case we need to push out updates, you will need to install them manually.

6. To work with Yalnix, put your code in the shared folder (so you can edit it easily and work with git).  
