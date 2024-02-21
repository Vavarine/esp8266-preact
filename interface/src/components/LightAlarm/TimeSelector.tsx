import { useEffect, useRef, useState } from "preact/hooks";
import { VerticalCarousel, VerticalCarouselItem } from "./VerticalCarousel";

interface TimeSelectorProps {
  hour?: number;
  minute?: number;
  onChange?: (hour: number, minute: number) => void;
}

export function TimeSelector({ hour, minute, onChange }: TimeSelectorProps) {
  const [selectedHour, setSelectedHour] = useState<number>(hour);
  const [selectedMinute, setSelectedMinute] = useState<number>(minute);

  const handleHourChange = (index: number) => {
    setSelectedHour(index);
  };

  const handleMinuteChange = (index: number) => {
    setSelectedMinute(index);
  };

  useEffect(() => {
    onChange?.(selectedHour, selectedMinute);
  }, [selectedHour, selectedMinute]);

  return (
    <div class="flex items-center">
      <VerticalCarousel defaultIndex={hour} onChange={handleHourChange}>
        {[...Array(24)].map((_, i) => {
          return (
            <VerticalCarouselItem index={i}>
              <span class="text-7xl">{i.toString().padStart(2, "0")}</span>
            </VerticalCarouselItem>
          );
        })}
      </VerticalCarousel>
      <span class="text-5xl">:</span>
      <VerticalCarousel defaultIndex={minute} onChange={handleMinuteChange}>
        {[...Array(60)].map((_, i) => {
          return (
            <VerticalCarouselItem index={i}>
              <span class="text-7xl">{i.toString().padStart(2, "0")}</span>
            </VerticalCarouselItem>
          );
        })}
      </VerticalCarousel>
    </div>
  );
}
