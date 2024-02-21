interface IconProps extends React.HTMLAttributes<HTMLDivElement> {
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
