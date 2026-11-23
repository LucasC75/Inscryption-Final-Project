# Inscryption-Final-Project

A simple text version of Inscryption written in C (blood costs only, no bones).

## Build and run
```
cd code
make
./inscryption
```

## Files
| File | Contents |
|------|----------|
| `main.c`   | Main menu, initialization, calls into the battle |
| `card.h` / `card.c`     | `MCard` struct, loading `card_definitions.txt`, card display, validation, sigil (type-bonus) calculations |
| `player.h` / `player.c` | `Player` struct, player setup, deck/hand management, card selection and sacrifices |
| `battle.h` / `battle.c` | Stack (decks), queue (opponent's incoming cards), board display, battle resolution, scale, winner |
| `card_definitions.txt`  | Card list: `Name, Health, Attack, Sigil, Blood cost` |

## Rules
- Before the battle you get a random 12-card deck rolled by rarity (rarity = blood cost):
  50% Common (0), 30% Uncommon (1), 15% Rare (2), 5% Epic (3-4). You may reroll up to 3 cards.
- Draw from your main deck or squirrel deck each turn.
- Pay a card's blood cost by sacrificing cards on your board (Black Goat = 3 blood).
- Ring the bell: each card attacks the lane in front of it; empty lanes send damage to the scale.
- Tip the scale to +5 to win, -5 loses.
