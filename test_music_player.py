#!/usr/bin/env python3
"""
Test script to verify that the music player can be imported and basic functionality works.
This does not test the GUI, but validates the code structure.
"""

import sys
import os

# Test imports
try:
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import Qt, QUrl
    from PyQt5.QtMultimedia import QMediaPlayer, QMediaPlaylist
    print("✓ PyQt5 imports successful")
except ImportError as e:
    print(f"✗ PyQt5 import failed: {e}")
    print("  Install with: pip install PyQt5")
    sys.exit(1)

try:
    from mutagen import File as MutagenFile
    from mutagen.id3 import ID3, APIC
    from mutagen.mp3 import MP3
    from mutagen.flac import FLAC
    print("✓ mutagen imports successful")
except ImportError as e:
    print(f"✗ mutagen import failed: {e}")
    print("  Install with: pip install mutagen")
    sys.exit(1)

# Test that the music player module can be imported
try:
    # Add the directory to path if needed
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import music_player
    print("✓ music_player module imports successfully")
except Exception as e:
    print(f"✗ music_player import failed: {e}")
    sys.exit(1)

# Test basic instantiation (without showing GUI)
try:
    app = QApplication(sys.argv)
    player = music_player.MusicPlayer()
    print("✓ MusicPlayer instance created successfully")
    
    # Test some basic properties
    assert hasattr(player, 'player'), "Missing player attribute"
    assert hasattr(player, 'playlist'), "Missing playlist attribute"
    assert hasattr(player, 'song_list'), "Missing song_list attribute"
    assert hasattr(player, 'lyrics_data'), "Missing lyrics_data attribute"
    print("✓ MusicPlayer has all required attributes")
    
    # Test basic methods exist
    methods = ['play', 'pause', 'stop', 'previous_song', 'next_song', 
               'add_songs', 'remove_song', 'load_lyrics', 'format_time']
    for method in methods:
        assert hasattr(player, method), f"Missing method: {method}"
    print(f"✓ All {len(methods)} required methods exist")
    
    # Test format_time function
    assert player.format_time(0) == "00:00", "format_time(0) failed"
    assert player.format_time(60000) == "01:00", "format_time(60000) failed"
    assert player.format_time(125000) == "02:05", "format_time(125000) failed"
    print("✓ format_time() works correctly")
    
except Exception as e:
    print(f"✗ MusicPlayer instantiation failed: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

print("\n" + "="*50)
print("All tests passed! ✓")
print("="*50)
print("\nTo run the music player GUI, execute:")
print("  python3 music_player.py")
