# CodeQuest

An interactive C / C++ learning and revision app, written in C with [raylib](https://www.raylib.com/).

CodeQuest is the next version of **CExam Trainer**. It keeps the same toolchain
(C99, raylib's bundled gcc, `build.bat` / `Makefile`) and adds a friendlier
blue-and-white interface: a dashboard, learning paths, lessons and quizzes.

![Dashboard](shots/01-dashboard.png)

---

## Stage 1 — Dashboard (this build)

- Native raylib window, blue-and-white layout
- Sidebar navigation
- Robot mascot drawn with raylib shapes
- Progress cards and learning-path cards
- Mouse interactions and toast messages

The numbers on the dashboard are placeholder data, and the sidebar items
other than **Dashboard** show a "coming in a later stage" toast.

## Planned stages

| Stage | Adds |
|---|---|
| 1 | Dashboard, sidebar, mascot *(done)* |
| 2 | Learning paths: C Fundamentals, Coding Exam 1, Coding Exam 2, Advanced C++ |
| 3 | Lessons built from the course notes |
| 4 | Quizzes (multiple choice + coding), reusing the CExam Trainer quiz engine |
| 5 | Saved progress feeding the dashboard |

---

## Project layout

```
src/
  main.c        window, app state, main loop
  ui.c/.h       palette, buttons, progress bars, toast, robot mascot
  sidebar.c/.h  left-hand navigation
  dashboard.c/.h  the home screen
build.bat       one-click Windows build
Makefile        make / make run / make debug / make shots / make clean
shots/          screenshots (regenerate with `make shots`)
```

## Building

Install raylib for Windows (it puts gcc in `C:\raylib\w64devkit`). Then either:

```bat
build.bat          :: build only
build.bat run      :: build then launch
```

or, with make:

```bat
make run
```

If raylib lives somewhere other than `C:\raylib`, set `RAYLIB_DIR` first:

```bat
set RAYLIB_DIR=D:\path\to\raylib
```

The executable is written to `build\CodeQuest.exe`.

## to launch got into terminal and type .\build.bat run