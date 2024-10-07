// import LedSwitch from "@/components/LedSwitch";
import LedSwitch from "@/components/LedSwitch";
import Temperature from "@/components/Temperature";

export default function Home() {
  return (
    <div className="min-h-screen items-stretch border-8">
      <div className="p-8 flex flex-col md:flex-row gap-8 items-center sm:items-start">
        <Temperature />
        <LedSwitch />
      </div>
      <div className="text-center fixed bottom-0 space-y-1 w-full mb-2">
        <p>Een project van Kees & Leo</p>
        <p>V0.5</p>
      </div>
    </div>
  );
}
