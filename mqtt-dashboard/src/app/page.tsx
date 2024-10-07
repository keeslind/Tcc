// import LedSwitch from "@/components/LedSwitch";
import LedSwitch from "@/components/LedSwitch";
import Temperature from "@/components/Temperature";

export default function Home() {
  return (
    <div>
      <div className="p-8 flex flex-col md:flex-row gap-8 items-center sm:items-start">
        <Temperature />
        <LedSwitch />
      </div>
      <div className="text-center p-4 mx-8 shadow-md border">
        <p>Een project van Kees & Leo</p>
        <p>V0.5</p>
      </div>
    </div>
  );
}
