# ATLAS - CONTROL SOFTWARE

This repo is the home of the controll software of ATLAS, it can be forked and modified or flashed on the custom ESP32 Main Controller Board.
## Prerequisites

Before getting started, install the following:

* **Visual Studio Code**
* **PlatformIO IDE** extension for VS Code

## Installation

1. Clone this repository:

```bash
git clone https://github.com/ATLAS-ROBOTIC-SYSTEMS/control-software
cd control-software
```

Or download the repository as a ZIP and extract it.

2. Open the project folder (the repository root) in **VS Codium**.

3. If prompted, allow PlatformIO to initialize the project.

> **Note:** All required project files, including `platformio.ini`, are already included in the repository root. No additional setup is required.

---

## Building the Filesystem (LittleFS)

Before uploading the firmware, build the LittleFS image.

### Using the PlatformIO Toolbar

1. Open the **PlatformIO** sidebar.
2. Select **Project Tasks**.
3. Expand your environment.
4. Under **Platform**, click:

```
Build Filesystem Image
```

Wait until the build completes successfully.

---

## Uploading the LittleFS Filesystem

1. Put the board into bootloader mode:

   * Press and hold the **BOOT** button.
   * While holding **BOOT**, press and release the **RST** (Reset) button.
   * Release the **BOOT** button.

2. In PlatformIO, click:

```
Upload Filesystem Image
```

Wait until the upload finishes.

---

## Building the Firmware

Click the **Build** button (✓) in the PlatformIO toolbar, or run:

```
PlatformIO: Build
```

Make sure the build completes without errors.

---

## Uploading the Firmware

1. Again, put the board into bootloader mode:

   * Hold **BOOT**
   * Press and release **RST**
   * Release **BOOT**

2. Click the **Upload** button (→) in PlatformIO.

Wait until the upload completes.

---

## Serial Monitor

To view debug output:

1. Click the **Serial Monitor** button in PlatformIO.
2. Select the correct COM port if necessary.
3. The baud rate is configured automatically from `platformio.ini`.

---

## Updating the Project

If you pull new changes from GitHub:

```bash
git pull
```

Rebuild both:

1. **Build Filesystem Image**
2. **Upload Filesystem Image**
3. **Build**
4. **Upload**

This ensures both the firmware and the web assets are updated.

---



### Upload fails

Try entering bootloader mode manually:

1. Hold **BOOT**
2. Press and release **RST**
3. Release **BOOT**
4. Start the upload immediately.

### LittleFS changes don't appear

If you modified any files inside the `data/` directory, you must:

1. Build the Filesystem Image.
2. Upload the Filesystem Image.

Uploading only the firmware will **not** update the filesystem.

### Build errors

* Verify PlatformIO is installed.
* Ensure the entire repository was opened (not just a subfolder).
* Confirm that `platformio.ini` is located in the repository root.



