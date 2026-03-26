# TERMSPN
### Sports stats for the terminal — no bloat, no ads, no browser required.

TERMSPN is a POSIX-compliant terminal application that pulls live standings and team data directly from ESPN's public API. Navigate leagues, browse standings, and drill into team details without ever leaving your shell.

![project screenshot](./assets/screenshot.png)

---

## Features

- **Live standings** — NBA and NFL standings pulled fresh from ESPN on every launch
- **Conference split** — East/West and AFC/NFC displayed in order, sorted by win percentage
- **Per-team detail** — overall record, home/away splits, division and conference records, last 10 games, PPG, OPP PPG, point differential, and current streak
- **Keyboard-driven navigation** — no mouse required
- **Streak coloring** — win streaks highlighted green, loss streaks red
- **Minimal dependencies** — ncurses and libcurl, nothing else

---

## Dependencies

| Library | Purpose |
|---|---|
| `ncurses` | Terminal rendering |
| `libcurl` | HTTP requests to ESPN API |
| `nlohmann/json` | JSON parsing (header-only, included) |

**Install dependencies:**

```bash
# Debian / Ubuntu
sudo apt install libncurses-dev libcurl4-openssl-dev

# macOS
brew install ncurses curl
```

---

## Installation

```bash
git clone https://github.com/yourname/termspn.git
cd termspn
make
```

| Command | Description |
|---|---|
| `make` | Compile the program |
| `make run` | Build (if needed) and run |
| `make clean` | Remove compiled files |

---

## Usage

Launch with:

```bash
./espn
```

### Controls

| Key | Action |
|---|---|
| `↑` / `↓` | Navigate up and down |
| `Enter` | Select league / view team detail |
| `Q` | Go back / quit |
| `Esc` | Return to standings from team detail |

### Navigation flow

```
League Select  →  Standings  →  Team Detail
      ↑               ↑              |
      |               └──────────────┘
      └─────── Q from Standings
```

---

## Data Source

All data is sourced from ESPN's public API (`site.api.espn.com`). No API key is required. Data reflects live standings and is fetched on each launch.

---

## Roadmap

- [ ] More leagues (MLB, NBA, NHL, MLS)
- [ ] Scores and live game updates
- [ ] Player stats view
- [ ] Configurable refresh interval
- [ ] Color theme support

---

## License

MIT
