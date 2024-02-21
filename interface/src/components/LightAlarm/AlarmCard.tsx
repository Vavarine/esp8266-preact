import { Alarm } from "@/types";
import { EditAlarmModal } from "./EditAlarmModal";

interface AlarmProps extends Alarm {
  onRemove: () => void;
  onEdit: (alarm: Alarm) => void;
}

export function AlarmCard({
  id,
  time,
  days,
  enabled,
  onRemove,
  onEdit,
}: AlarmProps) {
  const handleEnableChange = (e: Event) => {
    e.stopPropagation();
    onEdit({ id, time, days, enabled: !enabled });
  };

  return (
    <div
      class="card py-4 px-6 hover:bg-base-200 justify-between flex-row items-center"
      onClick={() =>
        (document.getElementById(id) as HTMLDialogElement).showModal()
      }
    >
      <div>
        <span class="text-2xl">{time.hour}</span>h
        <span class="text-2xl">{time.minute}</span>m
      </div>
      <div>
        <div class="flex align-center">
          {Object.keys(days).map((day) => {
            const isActive = days[day as keyof typeof days];

            return (
              <div class="indicator h-5 w-5 flex items-center justify-center">
                <span class={isActive ? "opacity-100" : "opacity-20"}>
                  {day.slice(0, 1)}
                </span>
                {isActive && (
                  <span className="indicator-item indicator-center badge p-0 w-1 h-1 badge-primary" />
                )}
              </div>
            );
          })}
        </div>
      </div>
      <input
        type="checkbox"
        checked={enabled}
        class="checkbox checkbox-primary"
        onClick={handleEnableChange}
      />
      <EditAlarmModal
        modalId={id}
        onRemove={onRemove}
        onEdit={onEdit}
        alarm={{
          id,
          days,
          enabled,
          time,
        }}
      />
    </div>
  );
}
