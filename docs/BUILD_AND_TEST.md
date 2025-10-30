# Build and Test Instructions

## Files to Add to CMakeLists.txt

Add these new files to your CMakeLists.txt:

```cmake
# In src/ sources
src/world/gen/LevelGenerator.cpp
src/world/gen/FeaturePlacer.cpp
```

## Compilation

```bash
cd build
cmake ..
make
```

## Expected Compilation Warnings/Errors

None expected. All files should compile cleanly.

## Quick Test Checklist

### 1. Launch Game
```bash
./build/rogueline  # or whatever your executable is named
```

### 2. First Level Test
- [ ] Game launches successfully
- [ ] Message shows "Generated level with X stairs" (X should be 1-3)
- [ ] Map displays correctly with '@' (player), 'g' (goblins), '>' (stairs), '+' (doors)

### 3. Stairs Test
- [ ] Use hjkl/WASD to move to stairs '>'
- [ ] Press '>' to descend
- [ ] Message shows "You descend the stairs..."
- [ ] New level generates
- [ ] Message shows "Welcome to depth 2!"
- [ ] Player HP is preserved

### 4. Door Test
- [ ] Move adjacent to a door '+'
- [ ] Press 'o' to open
- [ ] Message shows "Door opened"
- [ ] Door changes to '\'' (open)
- [ ] Can walk through open door

### 5. Look Mode Test
- [ ] Press 'x' to enter look mode
- [ ] Move cursor to stairs
- [ ] Info shows "Dist: X | Stairs down"
- [ ] Move cursor to door
- [ ] Info shows "Dist: X | Closed door" or "Open door"

### 6. Monster AI Test
- [ ] Wait near monsters (press '.')
- [ ] Monsters should move toward player
- [ ] Monsters respect closed doors (don't walk through)
- [ ] Monsters can walk through open doors

## Debug Output

The game currently has debug output in `descendStairs()`:
- "descendStairs() - start"
- "HP saved: X"
- "Position: X,Y"
- "Has stairs feature: true/false"
- etc.

This is intentional for testing. Remove when confident system works.

## Known Issues / Expected Behavior

### Stairs Count
- First stairs: guaranteed
- Second stairs: 15% chance
- Third stairs: 5% chance
- Typical: 1-2 stairs per level, occasionally 3

### Door Placement
- Only at corridor-room junctions
- Some rooms may have no doors (fully connected)
- Some corridors may have multiple doors

### Spawn Points
- Player spawns at least 5 tiles from any stairs
- Monsters spawn at least 5 tiles from stairs
- Up to 20 monsters per level (may be less if not enough space)

## Troubleshooting

### "Nothing to open here" when standing on door
**Cause**: Door opening code checks adjacent tiles, not current tile
**Solution**: Move adjacent to door, then press 'o'

### Stairs don't appear
**Cause**: No valid floor positions found
**Solution**: Check level generation - should not happen with RoomsGen

### Monsters walk through closed doors
**Cause**: FeatureManager not passed to AI
**Solution**: Verify all AI calls include `*featureMgr_` parameter

### Crash when pressing '>'
**Cause**: FeatureManager not initialized or null check failed
**Solution**: Verify `featureMgr_` is created in `generateLevel()`

## Performance Notes

- LevelGenerator creates all structures in single pass
- FeaturePlacer scans map once per feature type
- Expected generation time: <10ms for 60x40 map

## Success Confirmation

✅ Game launches without errors
✅ Stairs render as '>'
✅ Can descend stairs with '>'
✅ Depth counter increases
✅ HP preserved between levels
✅ Doors render as '+' (closed) or '\'' (open)
✅ Can open doors with 'o'
✅ Monsters don't walk through closed doors
✅ Look mode shows feature info correctly

If all checkboxes pass, migration is successful!

## Next Steps After Testing

1. Remove debug output from `descendStairs()`
2. Optional: Remove legacy Tile enum values (StairsDown, DoorClosed, DoorOpen)
3. Optional: Add connectivity validation
4. Optional: Make stairs placement configurable

## Reporting Issues

If you encounter issues:
1. Check console output for errors
2. Verify CMakeLists.txt includes new files
3. Clean build (`rm -rf build/*` then rebuild)
4. Check that all #include paths are correct
5. Verify line endings are consistent (Unix LF, not Windows CRLF)
