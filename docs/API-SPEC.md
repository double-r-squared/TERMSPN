# ESPN Public API Reference

**Unofficial API** — No auth required for most endpoints. Implement caching and error handling.

**Coverage:** 17 sports · 139 leagues · 370 v2 endpoints · 79 v3 endpoints

---

## Base URLs

| Domain | Version | Purpose |
|--------|---------|---------|
| `site.api.espn.com` | v2/v3 | Scores, news, teams, standings |
| `sports.core.api.espn.com` | v2 | Athletes, stats, odds, play-by-play |
| `sports.core.api.espn.com` | v3 | Athletes, leaders (richer schema) |
| `site.web.api.espn.com` | v3 | Search, athlete profiles |
| `cdn.espn.com` | — | CDN-optimized live data |
| `fantasy.espn.com` | v3 | Fantasy sports leagues |
| `now.core.api.espn.com` | — | Real-time news feeds |

---

## API Endpoint Patterns

### Site API v2 — Scores, Teams, Standings

```
GET https://site.api.espn.com/apis/site/v2/sports/{sport}/{league}/{resource}
```

| Resource | Description |
|----------|-------------|
| `scoreboard` | Live & scheduled events with scores |
| `teams` | All teams in the league |
| `teams/{id}` | Single team detail |
| `teams/{id}/roster` | Team roster |
| `teams/{id}/schedule` | Team schedule |
| `teams/{id}/depthcharts` | Depth chart by position |
| `teams/{id}/injuries` | Current injury report |
| `teams/{id}/transactions` | Recent transactions/moves |
| `teams/{id}/history` | Franchise historical record |
| `athletes/{id}` | Individual athlete profile |
| `athletes/{id}/gamelog` | Game-by-game log |
| `athletes/{id}/splits` | Statistical splits |
| `athletes/{id}/news` | Athlete news |
| `athletes/{id}/bio` | Athlete bio |
| `standings` | ⚠️ Use `/apis/v2/` instead — `/apis/site/v2/` returns a stub |
| `injuries` | League-wide injury report |
| `transactions` | Recent signings/trades/waivers |
| `groups` | Conferences/divisions |
| `news` | Latest news articles |
| `rankings` | Rankings (college sports) |
| `calendar` | Season calendar |
| `summary?event={id}` | Full game summary |

### Site API v3 — Richer Game Data

```
GET https://site.api.espn.com/apis/site/v3/sports/{sport}/{league}/{resource}
```

| Resource | Description |
|----------|-------------|
| `scoreboard` | Scoreboard with enriched v3 schema |
| `summary?event={id}` | Enriched game summary |

### Core API v2 — Athletes, Stats, Events, Odds

```
GET https://sports.core.api.espn.com/v2/sports/{sport}/leagues/{league}/{resource}
```

| Resource | Description |
|----------|-------------|
| `athletes` | Full athlete list with pagination |
| `athletes/{id}` | Single athlete |
| `athletes/{id}/statistics` | Career stats |
| `athletes/{id}/statisticslog` | Game-by-game log |
| `athletes/{id}/eventlog` | Event history |
| `athletes/{id}/contracts` | Contract info |
| `athletes/{id}/awards` | Awards |
| `athletes/{id}/hotzones` | Hot zones (baseball) |
| `athletes/{id}/injuries` | Athlete injury history |
| `athletes/{id}/vsathlete/{opponentId}` | Head-to-head stats |
| `events` | Events with full detail |
| `events/{id}/competitions/{id}/odds` | Betting odds |
| `events/{id}/competitions/{id}/probabilities` | Win probabilities |
| `events/{id}/competitions/{id}/plays` | Play-by-play |
| `events/{id}/competitions/{id}/situation` | Current game situation |
| `events/{id}/competitions/{id}/broadcasts` | Broadcast network info |
| `events/{id}/competitions/{id}/predictor` | ESPN game predictor |
| `events/{id}/competitions/{id}/powerindex` | ESPN Power Index for game |
| `events/{id}/competitions/{id}/competitors/{id}/linescores` | Period-by-period scores |
| `events/{id}/competitions/{id}/competitors/{id}/statistics` | Competitor stats |
| `seasons/{year}/teams` | Teams in a season |
| `seasons/{year}/coaches` | Coaching staff |
| `seasons/{year}/draft` | Draft data |
| `seasons/{year}/futures` | Futures odds |
| `seasons/{year}/powerindex` | Season-level Power Index / BPI |
| `seasons/{year}/types/{type}/groups/{group}/qbr/{split}` | ESPN QBR (football) |
| `standings` | League standings |
| `teams` | Teams (detailed) |
| `venues` | Venues/stadiums |
| `leaders` | Statistical leaders |
| `rankings` | Rankings |
| `franchises` | Franchise history |
| `coaches/{id}` | Individual coach profile |
| `coaches/{id}/record/{type}` | Coaching record by type |

### Core API v3 — Enriched Schema

```
GET https://sports.core.api.espn.com/v3/sports/{sport}/{league}/{resource}
```

| Resource | Description |
|----------|-------------|
| `athletes` | Athletes (enriched schema) |
| `athletes/{id}` | Single athlete (enriched) |
| `athletes/{id}/statisticslog` | Game log (enriched) |
| `athletes/{id}/plays` | Athlete play history |
| `leaders` | Statistical leaders |

### Search & Web API

```
GET https://site.web.api.espn.com/apis/{path}
```

| Endpoint | Description |
|----------|-------------|
| `/search/v2?query={q}&limit={n}` | Global ESPN search |
| `/search/v2?query={q}&sport={sport}` | Sport-scoped search |
| `/v2/scoreboard/header` | Scoreboard header/nav state |
| `/apis/common/v3/sports/{sport}/{league}/athletes/{id}/overview` | Athlete overview |
| `/apis/common/v3/sports/{sport}/{league}/athletes/{id}/stats` | Season stats (NFL/NBA/NHL/MLB) |
| `/apis/common/v3/sports/{sport}/{league}/athletes/{id}/gamelog` | Game-by-game log |
| `/apis/common/v3/sports/{sport}/{league}/athletes/{id}/splits` | Home/away/opponent splits |
| `/apis/common/v3/sports/{sport}/{league}/statistics/byathlete` | Stats leaderboard |

### CDN API — Real-Time Optimized

```
GET https://cdn.espn.com/core/{sport}/{resource}?xhr=1
```

Returns JSON when `xhr=1` is passed. Full game data is under the `gamepackageJSON` key.

| Endpoint | Description |
|----------|-------------|
| `/{sport}/scoreboard?xhr=1` | CDN-optimized live scoreboard |
| `/{sport}/scoreboard?xhr=1&league={league}` | Soccer scoreboard (e.g. `eng.1`) |
| `/{sport}/game?xhr=1&gameId={id}` | Full game package |
| `/{sport}/boxscore?xhr=1&gameId={id}` | Boxscore only |
| `/{sport}/playbyplay?xhr=1&gameId={id}` | Play-by-play only |

### Now API — Real-Time News

| Endpoint | Description |
|----------|-------------|
| `https://now.core.api.espn.com/v1/sports/news?limit={n}` | Global news feed |
| `...?sport={sport}&limit={n}` | Sport-filtered |
| `...?leagues={league}&limit={n}` | League-filtered |
| `...?team={abbrev}&limit={n}` | Team-filtered |

---

## Common League Slugs

### Football (`football`)
`nfl` · `college-football` · `cfl` · `ufl` · `xfl`

### Basketball (`basketball`)
`nba` · `wnba` · `nba-development` · `mens-college-basketball` · `womens-college-basketball` · `nbl` · `fiba`

### Baseball (`baseball`)
`mlb` · `college-baseball` · `world-baseball-classic`

### Hockey (`hockey`)
`nhl` · `mens-college-hockey` · `womens-college-hockey`

### Soccer (`soccer`)
`fifa.world` · `uefa.champions` · `eng.1` · `esp.1` · `ger.1` · `ita.1` · `fra.1` · `usa.1` · `mex.1` · `usa.nwsl` · `uefa.europa` · `fifa.wwc`

### Golf (`golf`)
`pga` · `lpga` · `eur` · `liv` · `champions-tour` · `ntw`

### Racing (`racing`)
`f1` · `irl` · `nascar-premier` · `nascar-secondary` · `nascar-truck`

### Tennis (`tennis`)
`atp` · `wta`

---

## Fantasy Sports API

```
GET https://fantasy.espn.com/apis/v3/games/{sport}/seasons/{year}
```

Game codes: `ffl` (football) · `fba` (basketball) · `flb` (baseball) · `fhl` (hockey)

```
GET /apis/v3/games/ffl/seasons/2024/segments/0/leagues/{league_id}
```

Views: `mTeam` · `mRoster` · `mMatchup` · `mMatchupScore` · `mSettings` · `mDraftDetail` · `mScoreboard` · `mStandings`

Private leagues require cookies: `espn_s2` and `SWID`

---

## Betting & Odds

Base: `https://sports.core.api.espn.com/v2/sports/{sport}/leagues/{league}`

| Endpoint | Description |
|----------|-------------|
| `/events/{id}/competitions/{id}/odds` | Game odds |
| `/events/{id}/competitions/{id}/probabilities` | Win probabilities |
| `/seasons/{year}/futures` | Season futures |
| `/seasons/{year}/types/{type}/teams/{id}/ats` | ATS records |
| `/seasons/{year}/types/{type}/teams/{id}/odds-records` | Team odds records |

Provider IDs: Caesars=38 · FanDuel=37 · DraftKings=41 · BetMGM=58 · ESPN BET=68 · Bet365=2000

---

## Query Parameters

| Parameter | Description | Example |
|-----------|-------------|---------|
| `dates` | Filter by date | `20241215` or `20241201-20241231` |
| `week` | Week number | `1`–`18` |
| `seasontype` | Season type | `1`=pre, `2`=regular, `3`=post, `4`=offseason |
| `season` | Year | `2024` |
| `limit` | Results limit | `100`, `1000` |
| `page` | Page number | `1` |
| `groups` | Conference ID | `8` (SEC) |
| `enable` | Inline-expand extra data | `roster`, `stats`, `injuries`, `projection` |
| `active` | Active filter | `true` / `false` |
| `lang` | Language / locale | `en`, `es`, `pt` |
| `region` | Regional content filter | `us`, `gb`, `au` |
| `xhr` | CDN JSON signal | `1` |

College Football Conference IDs: SEC=8 · Big Ten=5 · ACC=1 · Big 12=4 · Mountain West=17 · Top 25=80

---

## Notable Specialized Endpoints

### QBR (Quarterback Rating)
```
GET https://sports.core.api.espn.com/v2/sports/football/leagues/nfl/seasons/{year}/types/{type}/groups/{group}/qbr/{split}
GET https://sports.core.api.espn.com/v2/sports/football/leagues/nfl/seasons/{year}/types/{type}/weeks/{week}/qbr/{split}
```
`split`: `0`=totals, `1`=home, `2`=away

### Bracketology (NCAA Tournament)
```
GET https://sports.core.api.espn.com/v2/tournament/{tournamentId}/seasons/{year}/bracketology
GET https://sports.core.api.espn.com/v2/tournament/{tournamentId}/seasons/{year}/bracketology/{iteration}
```

### Power Index (BPI / SP+)
```
GET https://sports.core.api.espn.com/v2/sports/{sport}/leagues/{league}/seasons/{year}/powerindex
GET https://sports.core.api.espn.com/v2/sports/{sport}/leagues/{league}/seasons/{year}/powerindex/leaders
GET https://sports.core.api.espn.com/v2/sports/{sport}/leagues/{league}/seasons/{year}/powerindex/{teamId}
```

### Recruiting (College Football)
```
GET https://sports.core.api.espn.com/v2/sports/football/leagues/college-football/seasons/{year}/recruits
GET https://sports.core.api.espn.com/v2/sports/football/leagues/college-football/seasons/{year}/classes/{teamId}
```
