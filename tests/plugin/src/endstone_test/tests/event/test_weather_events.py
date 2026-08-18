from endstone_test.recorder import EventRecorder


def test_weather_change(recorder: EventRecorder) -> None:
    """Verify WeatherChangeEvent reports the new weather state."""
    for snapshot in recorder.require("WeatherChangeEvent"):
        assert isinstance(snapshot["to_weather_state"], bool)
        assert snapshot["level"]


def test_thunder_change(recorder: EventRecorder) -> None:
    """Verify ThunderChangeEvent reports the new thunder state."""
    for snapshot in recorder.require("ThunderChangeEvent"):
        assert isinstance(snapshot["to_thunder_state"], bool)
        assert snapshot["level"]
