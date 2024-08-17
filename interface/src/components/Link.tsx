import { useLocation } from 'preact-iso'
import type { JSX } from 'preact'

interface LinkProps extends JSX.HTMLAttributes<HTMLAnchorElement> {
	activeClass?: string
	activeClassName?: string
}

export function Link({
	class: c,
	className,
	activeClass,
	activeClassName,
	...props
}: LinkProps) {
	const inactive = [c, className].filter(Boolean).join(' ')
	const active = [c, className, activeClass, activeClassName]
		.filter(Boolean)
		.join(' ')
	const matches = useLocation().url === props.href

	return <a {...props} class={matches ? active : inactive} />
}
