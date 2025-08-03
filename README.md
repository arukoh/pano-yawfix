# Pano YawFix

## Overview

This is a stream-compatible CLI tool for **yaw (horizontal rotation) correction** of 360-degree video frames, designed to be used in combination with FFmpeg.  
Use it in a pipeline where FFmpeg decodes the video, `pano-yawfix` applies the correction, and FFmpeg encodes the result again.

## Main Features

- **🚀 High Performance**: OpenCL GPU acceleration (Intel/AMD/NVIDIA) + automatic CPU fallback
- **📊 Reliable Synchronization**: Complete frame number management with Y4M format
- **🌐 Cross-Platform**: Unified development environment for Windows/Linux (Dev Container supported)

## Technical Specifications

### Supported Formats & Performance

- **Input**: Y4M (YUV4MPEG2) pipeline / MP4 batch processing
- **Color Space**: YUV420P, YUV422P, YUV444P
- **Projection**: Equirectangular
- **Correction Data**: Per-frame yaw value (radian, CSV format)
- **Performance Target**: HD 60fps+, 4K 30fps+, 8K 15fps+

## Usage

### Quick Start

```bash
ffmpeg -i input.mp4 -f yuv4mpegpipe -pix_fmt yuv420p - | \
pano-yawfix stream --corrections corrections.csv | \
ffmpeg -f yuv4mpegpipe -i - -c:v libx264 -y output.mp4
```

### Main Options

```bash

# Required parameter
--corrections <file>   # Correction value file path

# Options
--opencl-mode <mode>   # OpenCL usage mode (auto|force|disable)
--report, -r <target>  # Target name for detailed log output
--verbose, -v          # Detailed log output (progress/error/info in JSON to stderr)
--help, -h             # Show this help
```

### Correction Data Format

```csv
 # corrections.csv - Per-frame yaw correction value (radian)
0.0
0.0174
0.0349
0.0523
0.0349
0.0174
0.0
-0.0174
-0.0349
```

### System Architecture

```text
[Input Video] → [FFmpeg Decode] → [Y4M Stream] → [Yaw Correction] → [Y4M Output] → [FFmpeg Encode] → [Output Video]
           ↗                                      ↗
      [Correction CSV]                          [OpenCL/CPU Processing]
```

## Development & Build

### Development Environment

```bash

# Using Dev Container (recommended)
code .  # VS Code Dev Container auto start

# Or manual environment setup
task setup    # Install dependencies
task build    # Build
task test     # Run tests
```

### Project Structure

```text
pano-yawfix/
├── src/              # Main implementation
├── include/          # Header files
├── tests/            # Unit & integration tests (29 files)
├── scripts/          # Automation scripts
├── build/            # Build artifacts
├── .devcontainer/    # Dev environment definition
└── Taskfile.yml      # Task automation
```

### Quality Management

```bash
task test              # Run all tests
task test:memcheck     # Memory leak check
task lint              # Static analysis
```
