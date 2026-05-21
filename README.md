# Chess Bot

## VSCode Setup

`c_cpp_properties.json` defines a configuration for the C/C++ extension, which allows a later C++ version to be used for Intellisense.

TODO: Windows version for this, and other files.

`tasks.json` contains a task where VSCode attempts to open Terminal.app, which triggers a permissions popup asking if you will allow VSCode to control Terminal. This is needed for the external terminal to be used when debugging, which is helpful for stdin. This is a bug that only occurs on MacOS, where normally VSCode will not trigger the permission popup.

`settings.json` sets the directory for the CMake Tools extension. This is `bots/`.

`launch.json` defines a debugging configuration that launches and attaches to the binary located at `bots/bin/Random Move Bot`.

`CMakePresets.json` defines **configure presets** for cmake, different from build presets. The difference corresponds to the two steps that you would normally take to manually build a cmake project: configure then build. **Configure** generates, for example, Makefiles, and **build** actually invokes make to build it. `CMakePresets.json` contains `Debug` and `Release` presets.