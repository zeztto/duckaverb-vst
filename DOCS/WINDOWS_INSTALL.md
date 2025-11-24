# Duckaverb - Windows Installation Guide

## System Requirements

- **OS**: Windows 10 or later (64-bit)
- **DAW**: Any VST3-compatible DAW (Reaper, FL Studio, Ableton Live, Cubase, etc.)
- **Architecture**: x64 (64-bit)

## Installation Options

Duckaverb is available in two formats for Windows:

1. **VST3 Plugin** - For use in DAWs
2. **Standalone App** - Standalone audio application

---

## VST3 Plugin Installation

### Step 1: Download

Download `Duckaverb-v0.0.1-Windows-VST3.zip` from the [GitHub Releases](https://github.com/zeztto/duckaverb-vst/releases) page.

### Step 2: Extract

1. Right-click the downloaded `.zip` file
2. Select **Extract All...**
3. Choose a temporary extraction location

### Step 3: Install

Copy the extracted `Duckaverb.vst3` folder to one of these locations:

**Recommended (System-wide)**:
```
C:\Program Files\Common Files\VST3\
```

**Alternative (User-specific)**:
```
C:\Users\YourUsername\AppData\Local\Programs\Common\VST3\
```

### Step 4: Rescan in DAW

1. Open your DAW (Reaper, Ableton, FL Studio, etc.)
2. Rescan for new plugins:
   - **Reaper**: Options > Preferences > Plug-ins > VST > Re-scan
   - **Ableton Live**: Preferences > Plug-ins > Rescan
   - **FL Studio**: Channels > Add > Refresh > Fast scan
   - **Cubase**: Studio > VST Plug-in Manager > Update Plug-in Information

3. Look for **Duckaverb** by **h4ppy Labs** in your plugin list

---

## Standalone App Installation

### Step 1: Download

Download `Duckaverb-v0.0.1-Windows-Standalone.zip` from the [GitHub Releases](https://github.com/zeztto/duckaverb-vst/releases) page.

### Step 2: Extract

1. Right-click the downloaded `.zip` file
2. Select **Extract All...**
3. Choose your installation location (e.g., `C:\Program Files\Duckaverb\`)

### Step 3: Run

1. Navigate to the extraction folder
2. Double-click `Duckaverb.exe` to launch

**Optional**: Create a desktop shortcut:
- Right-click `Duckaverb.exe`
- Select **Send to > Desktop (create shortcut)**

---

## Audio Configuration (Standalone)

When running Duckaverb as a standalone app:

1. Click **Options** (or **File > Audio Settings**)
2. Configure:
   - **Audio Device**: Select your audio interface
   - **Sample Rate**: 44100 Hz or 48000 Hz recommended
   - **Buffer Size**: 256 or 512 samples (adjust for latency)
   - **Input Channels**: Select your guitar input
   - **Output Channels**: Select your output

---

## Troubleshooting

### Plugin Not Showing in DAW

1. **Verify Installation Path**: Ensure `Duckaverb.vst3` is in the correct VST3 folder
2. **Check DAW VST3 Scan Path**: Make sure your DAW is scanning the VST3 folder
3. **Run as Administrator**: Try installing with administrator privileges
4. **Rescan Plugins**: Force a full plugin rescan in your DAW

### "Windows Protected Your PC" Warning

If Windows SmartScreen blocks the app:

1. Click **More info**
2. Click **Run anyway**

This warning appears because the plugin is not digitally signed with a commercial certificate.

### Standalone App Won't Launch

1. **Install Visual C++ Redistributable**:
   - Download from [Microsoft](https://aka.ms/vs/17/release/vc_redist.x64.exe)
   - Install and restart

2. **Check Antivirus**: Some antivirus software may block unsigned executables
   - Add `Duckaverb.exe` to your antivirus exceptions

### No Audio in Standalone

1. **Check Audio Settings**: Ensure correct audio device is selected
2. **Check Sample Rate**: Match your interface's sample rate
3. **Update Audio Drivers**: Install the latest ASIO drivers for your interface

---

## Usage

### One-Knob Control

- **SPACE Knob** (0% - 100%):
  - **0%**: Dry signal, no reverb
  - **50%**: Natural reverb with moderate ducking
  - **100%**: Massive ambient reverb with strong ducking

### Bypass Button

- **OFF** (Gray): Effect active
- **ON** (Red): Bypass mode, dry signal only

### Ducking Behavior

- When you play guitar, the reverb level automatically decreases (ducks) to keep your notes clear
- When you stop playing, the reverb swells up to fill the space
- Higher SPACE values = stronger ducking effect

---

## Uninstallation

### VST3 Plugin

Delete the plugin folder:
```
C:\Program Files\Common Files\VST3\Duckaverb.vst3
```

Then rescan plugins in your DAW.

### Standalone App

Simply delete the extracted folder containing `Duckaverb.exe`.

---

## Support

- **Issues**: [GitHub Issues](https://github.com/zeztto/duckaverb-vst/issues)
- **Documentation**: [GitHub Repository](https://github.com/zeztto/duckaverb-vst)

---

© 2025 h4ppy Labs. All rights reserved.
