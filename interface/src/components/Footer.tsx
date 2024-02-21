import { useLocation } from "preact-iso";

export function Footer() {
  const { url } = useLocation();

  const menuItems = [
    {
      name: "Remote Control",
      url: "/",
    },
    {
      name: "Other Page",
      url: "/other-page",
    },
  ];

  return (
    <ul className="menu menu-horizontal gap-2 bg-base-200 rounded-box w-max fixed bottom-4 left-1/2 -translate-x-1/2">
      {menuItems.map((item) => (
        <li class="">
          <a href={item.url} className={url === item.url ? "active" : ""}>
            {item.name}
          </a>
        </li>
      ))}
    </ul>
  );
}
