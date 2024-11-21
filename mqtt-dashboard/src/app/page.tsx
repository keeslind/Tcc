import Switch from "@/components/Switch";
import Train from "@/components/Train";

export default function Home() {
  return (
    <div className="min-h-screen items-stretch">
      <div className="p-8 flex flex-col md:flex-row gap-8 items-center sm:items-start">
        <Train />
        <Switch />
      </div>
      <div className="text-center fixed bottom-0 space-y-1 w-full mb-2">
        <p>Een project van Kees & Leo</p>
        <p>V0.5</p>
      </div>
    </div>
  );
}
