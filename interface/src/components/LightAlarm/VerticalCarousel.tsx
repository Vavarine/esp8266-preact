import { PropsWithChildren } from "preact/compat";
import { useEffect, useRef, useState } from "preact/hooks";

interface VerticalSliderProps extends PropsWithChildren {
  defaultIndex?: number;
  onChange?: (index: number) => void;
}

const isFullyVisibleOnYAxis = (item: DOMRect, container: DOMRect) => {
  return item.top >= container.top && item.bottom <= container.bottom;
};

const isHTMLElement = (element: unknown): element is HTMLElement => {
  return element instanceof HTMLElement;
};

function throttle(callbackFn, limit) {
  let wait = false;
  return function () {
    if (!wait) {
      callbackFn.call();
      wait = true;
      setTimeout(function () {
        wait = false;
      }, limit);
    }
  };
}

export function VerticalCarouselItem({
  children,
  index,
}: PropsWithChildren<{ index: number }>) {
  return (
    <div
      class="carousel-item opacity-20 data-[visible=true]:opacity-100 transition-opacity"
      // class="carousel-item border border-solid border-blue-900"
      data-index={index}
    >
      <div class="flex justify-center items-center h-full">{children}</div>
    </div>
  );
}

export function VerticalCarousel({
  children,
  defaultIndex: activeIndex,
  onChange,
}: VerticalSliderProps) {
  const ref = useRef<HTMLDivElement>(null);
  const [carouselItems, setCarouselItems] = useState<Element[]>([]);
  const visibleItems = useRef<Element[]>([]);

  let timer = null;

  const onScrollEnd = () => {
    const container = ref?.current;
    const activeItem = container.querySelector(
      ".carousel-item[data-visible=true]"
    );
    const activeIndex = parseInt(activeItem.getAttribute("data-index"));

    onChange?.(activeIndex);
  };

  const onScroll = () => {
    clearTimeout(timer);

    const container = ref?.current;

    if (!container) return;

    const containerRect = container.getBoundingClientRect();
    const currentItems = container.querySelectorAll(".carousel-item");

    currentItems.forEach((item) => {
      const rect = item.getBoundingClientRect();
      const isVisible = isFullyVisibleOnYAxis(rect, containerRect);

      item.setAttribute("data-visible", isVisible.toString());
    });

    const currentItemHeight = currentItems[0].getBoundingClientRect().height;

    if (
      container.scrollTop >
      container.scrollHeight - (currentItemHeight * carouselItems.length) / 4
    ) {
      carouselItems.forEach((item) => {
        container.appendChild(item.cloneNode(true));
      });
    }

    if (container.scrollTop < (currentItemHeight * carouselItems.length) / 4) {
      [...carouselItems].reverse().forEach((item) => {
        container.prepend(item.cloneNode(true));
      });
    }

    timer = setTimeout(onScrollEnd, 500);
  };

  const scrollTo = (index: number) => {
    const items = [
      ...ref?.current.querySelectorAll<HTMLDivElement>(".carousel-item"),
    ];

    if (!items) return;

    const activeItemIndex = items.findIndex(
      (item) => item.getAttribute("data-visible") === "true"
    );

    const activeItem = items[activeItemIndex];
    const possibleTargets = items.filter(
      (item) => item.getAttribute("data-index") === index.toString()
    );

    const nearestTarget = possibleTargets.reduce((prev, curr) => {
      const prevRect = prev.getBoundingClientRect();
      const currRect = curr.getBoundingClientRect();

      const prevDistance = Math.abs(
        prevRect.top - ref?.current.getBoundingClientRect().top
      );
      const currDistance = Math.abs(
        currRect.top - ref?.current.getBoundingClientRect().top
      );

      return prevDistance < currDistance ? prev : curr;
    });

    if (!isHTMLElement(activeItem)) return;

    const isVisible =
      nearestTarget.getAttribute("data-index") ===
      activeItem.getAttribute("data-index");

    if (!isVisible) {
      const container = ref?.current;
      const containerRect = container.getBoundingClientRect();
      const nearestTargetRect = nearestTarget.getBoundingClientRect();

      container.scrollTo({
        top:
          nearestTarget.offsetTop -
          container.offsetTop -
          containerRect.height / 2 +
          nearestTargetRect.height / 2,
        behavior: "instant",
      });
    }
  };

  const setupInitialInfiniteScroll = () => {
    const container = ref?.current;

    if (!container) return;

    [...carouselItems].reverse().forEach((item) => {
      container.prepend(item.cloneNode(true));
    });
    carouselItems.forEach((item) => {
      container.appendChild(item.cloneNode(true));
    });
  };

  useEffect(() => {
    if (!ref?.current) return;

    onScroll();
    ref.current.addEventListener("scroll", throttle(onScroll, 50));

    setCarouselItems([...ref.current.querySelectorAll(".carousel-item")]);

    return () => {
      ref.current.removeEventListener("scroll", throttle(onScroll, 50));
    };
  }, [ref.current]);

  useEffect(() => {
    if (activeIndex !== undefined) {
      scrollTo(activeIndex);
    }
  }, [activeIndex]);

  return (
    <div
      ref={ref}
      // class="h-44 overflow-y-scroll rounded-box"
      class="h-44 space-y-4 py-[50%] carousel carousel-center carousel-vertical rounded-box"
    >
      {children}
    </div>
  );
}
