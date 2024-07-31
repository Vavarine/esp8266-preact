import type { JSX } from "preact";

interface IconProps extends JSX.HTMLAttributes<HTMLDivElement> {
  icon: string;
}

export default function Icon({ icon, ...rest }: IconProps) {
  return (
    <div
      {...rest}
      dangerouslySetInnerHTML={{
        __html: icon,
      }}
    />
  );
}
