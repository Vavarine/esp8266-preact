import { useEffect, useState } from "preact/hooks";
import sunFilledIcon from "@assets/sun-filled.svg?raw";
import sunIcon from "@assets/sun.svg?raw";
import Icon from "@components/Icon";

export function RemoteControl() {
  const [ledState, setLedState] = useState(false);

  const getLedState = async () => {
    const response = await fetch("/api/led");
    const data = await response.json();

    setLedState(Boolean(data.state));
  }
  
  const handleButtonClick = async () => {
    const response = await fetch("/api/led/toggle");
    const data = await response.json();

    setLedState(Boolean(data.state));
  };

  useEffect(() => {
    getLedState();
  }, []);

  return (
    <div class="container py-8 flex flex-col justify-center items-center">
      <button
        class="btn btn-lg btn-circle col-start-1 group"
        onClick={handleButtonClick}
        data-active={ledState}
      >
        <Icon class="group-data-[active='false']:hidden" icon={sunFilledIcon} />
        <Icon class="group-data-[active='true']:hidden" icon={sunIcon} />
      </button>
    </div>
  )
}
