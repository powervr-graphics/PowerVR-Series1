*
* NOTE:
* This is a license from 3DCC an application distributed as part of the
* the documentation, it is an executable that we do not own and do not
* supply the source code for.
*
* The below license statement applies to the following files only!
* PowerVR-Series1/Documentation/sgl/Drivers/files/410c3X.zip/3dcc.zip
* PowerVR-Series1/Documentation/sgl/Drivers/files/411c3X.zip/3dcc.zip
*
* The rest of the repository remains licensed under the License file
* inside the root of the repo
*

This software and documentation are protected by copyright law,
with all rights reserved. Copyright is held by the author, Jayeson
Lee-Steere.

This software is provided free of charge and "as is". No warranties are
made, express or implied, and the author does not warrant that the
software will be fit for any particular purpose. The author will in no
event be liable for loss of profits, nor incidental or consequential
damages. You use this software at your own risk. This software is not
supported in any way by the author.

This software may not be modified, disassembled or reverse engineered
in any way without written authorization from the author.

This software may be distributed in unmodified form free of charge,
provided it is always distributed with this document and no charge is
made for it without permission from the author, with the exclusion of
reasonable distribution and/or service charges.

It is a condition of use that you agree to the above terms.

PowerVR is a trademark of NEC Electronics Inc.

Microsoft, Windows and Direct3D are either registered trademarks or
trademarks of Microsoft Corporation in the United States and/or other
countries.

Any other trademarks and/or registered trademarks are the property of their
respective owners.


INTRODUCTION

3C Control Center is a small application which allows you to easily use
multiple Direct3D accelerator cards in one system at the same time. It
places a "3D" icon the taskbar which quickly allows you to select which
accelerator card you want to use for Direct3D acceleration. Native 3D
applications are not affected by 3D Control Center - they will always
run on the appropriate accelerator card.

USAGE

Place 3DCC.EXE in a convenient directory (such as your Windows directory)
and run it. The "About/Options" box should appear, at which time you can
select if you always want 3D Control Center to load on Windows startup.
After clicking "Ok" a "3D" icon should appear on the taskbar.

Clicking on the taskbar icon brings up a menu which allows you to either
bring up the "About/Options" box or select a different device for Direct3D
acceleration. It is recommended that you do not change the Direct3D
acceleration device while a Direct3D application is running as the
application may not take note of the change and hence it may malfunction. 
The available devices are as follows:

PowerVR - A 3D accelerator device based on one of the PowerVR chipsets.

Secondary Video Device - This refers to the class of add in 3D accelerator
device which has its own video output, and is normally connected to the
monitor via a pass through cable.

Primary Video Device - This refers to your standard 2D video card, which
may or may not have 3D acceleration. If it does not have 3D acceleration,
selecting this option will disable all Direct3D acceleration.
