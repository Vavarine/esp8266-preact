import { Alarm } from "@/types";
import { TimeSelector } from "./TimeSelector";

interface EditAlarmModalProps {
  modalId: string;
  alarm?: Alarm;
  onRemove: () => void;
  onEdit: (alarm: Alarm) => void;
}

export function EditAlarmModal({
  modalId,
  onEdit,
  onRemove,
  alarm: { days, enabled, time, id },
}: EditAlarmModalProps) {
  return (
    <dialog id={modalId} className="modal modal-bottom sm:modal-middle">
      <div className="modal-box">
        <form method="dialog">
          <button className="btn btn-sm btn-circle btn-ghost absolute right-2 top-2">
            ✕
          </button>
        </form>
        <div class="flex items-center flex-col gap-6">
          <h3 class="text-xl mb-2">Alarm</h3>
          <TimeSelector
            hour={time.hour ?? new Date().getHours()}
            minute={time.minute ?? new Date().getMinutes()}
            onChange={(hour, minute) => {
              onEdit({ days, enabled, time: { hour, minute }, id });
            }}
          />
          <div class="flex gap-2 mt-4">
            {Object.keys(days).map((day) => {
              const isActive = days[day as keyof typeof days];

              return (
                <button
                  class={`btn btn-circle btn-sm ${
                    isActive ? "btn-primary" : ""
                  }`}
                  onClick={(e) => {
                    e.preventDefault();
                    // remove focus
                    e.currentTarget.blur();
                    const newDays = { ...days };
                    newDays[day as keyof typeof days] = !isActive;

                    onEdit({ days: newDays, time, enabled, id });
                  }}
                >
                  {day.slice(0, 1)}
                </button>
              );
            })}
          </div>
          <form method="dialog" class="w-full">
            <button class="btn btn-outline btn-error w-full" onClick={onRemove}>
              Remove
            </button>
          </form>
        </div>
      </div>
      <form method="dialog" className="modal-backdrop">
        <button>close</button>
      </form>
    </dialog>
  );
}
