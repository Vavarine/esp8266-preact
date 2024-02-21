import { useLocation } from "preact-iso";
import boxIcon from "@assets/box.svg?raw";
import moreHorizontalIcon from "@assets/more-horizontal.svg?raw";
import Icon from "./Icon";

export function Header() {
  const { url } = useLocation();

  return (
    <header class="">
      <div class="navbar rounded-b-xl shadow-xl overflow-visible">
        <div class="flex-1 ">
          <a class="btn btn-ghost text-xl gap-2">
            <Icon icon={boxIcon} size={24} />
            Esp8266 Preact
          </a>
        </div>
        <div class="flex-none dropdown dropdown-end">
          <div class="btn btn-square btn-ghost" role="button" tabIndex={0}>
            <Icon icon={moreHorizontalIcon} />
          </div>
          <ul class="menu menu-sm dropdown-content shadow rounded-box mt-4 bg-base-200">
            <li>
              <a href="/">Logout</a>
            </li>
          </ul>
        </div>
      </div>
    </header>
  );
}
