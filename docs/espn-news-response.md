# ESPN News API — Response Schema Reference

Two endpoints provide news. They share a common article shape but differ in wrapper structure and available fields.

---

## Endpoints

| Name | URL | Filtered? | Article key |
|------|-----|-----------|-------------|
| **Site API** | `https://site.api.espn.com/apis/site/v2/sports/basketball/nba/news` | Yes — NBA only | `articles[]` |
| **Now API** | `https://now.core.api.espn.com/v1/sports/news?leagues=nba&limit=25` | Yes via param | `headlines[]` |

**Recommendation:** Use **Site API** for league-specific news (cleaner filtering). Use **Now API** if you need `byline`, `story` body, or `keywords`.

---

## Top-Level Wrappers

### Site API
```json
{
  "header": "NBA News",
  "link": { ... },       // league nav link — skip
  "articles": [ ... ]
}
```

### Now API
```json
{
  "resultsCount": 25,
  "resultsLimit": 25,
  "resultsOffset": 0,    // use for pagination: ?limit=25&offset=25
  "headlines": [ ... ]
}
```

---

## Article Object — Field Comparison

### Shared Fields (both endpoints)

| Field | Type | Notes |
|-------|------|-------|
| `id` | int | Unique article ID |
| `type` | string | `"Story"` = full article · `"HeadlineNews"` = brief blurb |
| `headline` | string | **Primary display text** |
| `description` | string | **Short summary** — best for list view subtitles |
| `published` | string | ISO 8601 UTC — **use this for display date** |
| `lastModified` | string | ISO 8601 UTC |
| `premium` | bool | `true` = ESPN+ paywalled |
| `images[]` | array | See Images section below |
| `categories[]` | array | League / team / athlete tags — see below |
| `links.web.href` | string | Full ESPN.com article URL |
| `links.api.self.href` | string | `https://content.core.api.espn.com/v1/sports/news/{id}` — fetch full article |

### Now API Only

| Field | Type | Notes |
|-------|------|-------|
| `byline` | string | **Author name** — e.g. `"Kyle Bonagura"`. Missing on some `HeadlineNews`; fall back to `source` |
| `story` | string | **Full article body as HTML** — strip tags for terminal. Contains placeholder tokens (`<photo1>`, `<inline1>`, `<alsosee>`) — strip these too |
| `section` | string | Human-readable sport/league label e.g. `"NBA"`, `"MLB"` |
| `root` | string | Short sport slug e.g. `"nba"`, `"mlb"` |
| `keywords[]` | array | String tags e.g. `["NBA", "LeBron James"]` — good for filtering |
| `related[]` | array | Linked articles (partial shape — no `story` body) |
| `isLiveBlog` | bool | Whether the article is a live updating blog |

### Site API Only

| Field | Type | Notes |
|-------|------|-------|
| *(none unique)* | — | Site API articles are a subset of the Now API article shape |

> **Note:** Site API articles do **not** include `byline`, `story`, `section`, `root`, or `keywords`. To get the full article body, use `links.api.self.href` to fetch from the content API.

---

## Images

Both endpoints use the same image structure. Two types appear:

| `type` value | Description | Terminal |
|-------------|-------------|---------|
| `"header"` | Main article photo | Skip (URL available if needed) |
| `"Media"` | Video thumbnail | **Skip** — video not usable in terminal |

```
images[0].url      → CDN image URL
images[0].caption  → caption text
images[0].credit   → photo credit e.g. "AP Photo/..."
```

---

## Categories Array

Each item has a `type` field. Filter by type to extract useful data:

| `type` | Useful fields | Use for |
|--------|--------------|---------|
| `"league"` | `description`, `leagueId` | Label/filter by sport league |
| `"team"` | `description`, `teamId` | Tag articles with team names |
| `"athlete"` | `description`, `athleteId` | Tag articles with player names |
| `"topic"` | `description` | General tags like `"news"`, `"injury"` |
| `"guid"` | — | **Skip** — internal cross-reference only |

---

## Fields to Skip (Both Endpoints)

| Field | Reason |
|-------|--------|
| `nowId`, `contentKey`, `dataSourceIdentifier`, `publishedkey` | Internal ESPN IDs |
| `feedDisplayType` | ESPN app UI hint — always `"Default"` |
| `title`, `linkText` | Reworded duplicates of `headline` |
| `categorized`, `originallyPosted` | Use `published` instead |
| `links.mobile`, `links.app` | Mobile/app deep links |
| `allowAMP`, `allowAds`, `allowCommerce`, `allowComments`, `allowSearch`, `allowContentReactions` | ESPN platform flags |
| All `"guid"` type categories | Internal only |
| `images` where `type == "Media"` | Video thumbnails |
| Top-level `link` (Site API) | League nav — not needed |

---

## Recommended Extraction

### List View (both endpoints)
```python
{
    "id":          article["id"],
    "headline":    article["headline"],
    "description": article.get("description", ""),
    "published":   article["published"],        # parse to datetime, convert to local
    "type":        article["type"],             # Story vs HeadlineNews
    "url":         article["links"]["web"]["href"],
    "premium":     article["premium"],
    "teams":       [c["description"] for c in article.get("categories", []) if c["type"] == "team"],
    "athletes":    [c["description"] for c in article.get("categories", []) if c["type"] == "athlete"],
}
```

### Detail View (Now API only, or after fetching content API)
```python
{
    "author":      article.get("byline") or article.get("source", "ESPN"),
    "section":     article.get("section", ""),
    "keywords":    article.get("keywords", []),
    "is_live":     article.get("isLiveBlog", False),
    "story_html":  article.get("story", ""),    # html.unescape() then strip tags
    "related":     article.get("related", []),  # list of partial article objects
}
```

> To get the full `story` body from the Site API, fetch: `https://content.core.api.espn.com/v1/sports/news/{id}`

---

## Filtering — Video-Only Articles

The Site API mixes text articles and video highlight clips in the same response. Video articles have no readable content and should be excluded from any news list view.

### Detection (reliable)

Check `images[]` — video-only articles contain images but **none with `type: "header"`**. They only carry `type: "Media"` entries (video thumbnail frames).

```python
has_header = any(img["type"] == "header" for img in article.get("images", []))
if images present and not has_header → video-only, skip
```

### Why not match on headline strings?

String matching (e.g. `": Game Highlights"`, `"Watch:"`) is brittle — ESPN changes phrasing and the same pattern can appear in legitimate text articles. The `images[].type` check is structural and generalizes to all video formats (game highlights, film room, press conferences, etc.) without maintaining a keyword list.

### Limit

The Site API news endpoint defaults to a low article count. Use `?limit=50` to increase the pool before filtering:

```bash
GET https://site.api.espn.com/apis/site/v2/sports/basketball/nba/news?limit=50
```

Even with `limit=50`, video articles can consume a significant portion of results. Always request more than you need to ensure a usable list after filtering.

---

## Story HTML Notes

The `story` field (Now API / content API) is HTML with escaped entities (`\u003Cp\u003E` = `<p>`).

Strip these placeholder tokens before rendering:
- `<photo1>`, `<photo2>`, etc. — embedded images
- `<inline1>`, `<inline2>`, etc. — embedded widgets
- `<alsosee>` — related article callout
- `<video1>`, etc. — video embeds

Pipeline: `html.unescape(story)` → strip placeholder tokens → strip HTML tags → wrap text for terminal width.
