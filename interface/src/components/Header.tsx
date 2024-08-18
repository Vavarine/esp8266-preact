import boxIcon from '@assets/box.svg?raw'
import moreHorizontalIcon from '@assets/more-horizontal.svg?raw'
import Icon from './Icon'
import { useAuth } from '@/hooks/auth'
import { Link } from './Link'

export function Header() {
	const { signOut, user } = useAuth()

	return (
		<header class="">
			<div class="navbar rounded-b-xl shadow-xl overflow-visible">
				<div class="flex-1 ">
					<Link href="/" class="btn btn-ghost text-xl gap-2">
						<Icon icon={boxIcon} />
						Esp8266 Preact
					</Link>
				</div>
				<div class="flex gap-4">
					{!user?.username ? (
						<Link href="/login" class="btn">
							Log in
						</Link>
					) : (
						<>
							<span class="hidden sm:block">Hello, {user?.username}!</span>
							<div class="flex-none dropdown dropdown-end">
								<div
									class="btn btn-square btn-ghost"
									role="button"
									tabIndex={0}
								>
									<Icon icon={moreHorizontalIcon} />
								</div>
								<ul class="menu menu-sm dropdown-content shadow rounded-box mt-4 bg-base-200">
									<li>
										<span class="block max-w-48 w-full truncate sm:hidden">
											Hello, {user?.username}!
										</span>
									</li>
									{user && !user.noAuth && (
										<li>
											<button type="button" onClick={signOut}>
												Logout
											</button>
										</li>
									)}
								</ul>
							</div>
						</>
					)}
				</div>
			</div>
		</header>
	)
}
