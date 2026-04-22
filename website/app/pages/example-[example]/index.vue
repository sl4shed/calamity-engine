<script setup>
import { codeToHtml } from "shiki";

const route = useRoute();
const exampleId = route.params.example;

const highlightedCode = ref("");
const canvasRef = ref(null);
const moduleInstance = ref(null);
const open = ref(false);
const consoleOutput = ref(null);

const { data: manifest } = await useFetch(
  `examples/${exampleId}/manifest.json`,
  { server: false },
);

const activeTab = ref("main.cpp");
const fileTabItems = computed(() => {
  const files = manifest.value?.sourceFiles;
  if (!files) return [];

  return files.map((file) => ({
    label: file,
    value: file,
    icon: "i-lucide-file",
  }));
});

watchEffect(() => {
  const file = activeTab.value;
  if (!file) return;

  (async () => {
    try {
      const res = await fetch(`/examples/${exampleId}/${file}`);
      const text = await res.text();

      highlightedCode.value = await codeToHtml(text, {
        lang: "cpp",
        theme: "dark-plus",
      });
    } catch (err) {
      console.error("Failed to load file:", file, err);
      highlightedCode.value = "";
    }
  })();
});

const waitForCanvas = () => {
  return new Promise((resolve) => {
    const check = () => {
      if (canvasRef.value instanceof HTMLCanvasElement) {
        resolve(canvasRef.value);
      } else {
        requestAnimationFrame(check);
      }
    };
    check();
  });
};

const loadModule = async () => {
  const canvas = await waitForCanvas();
  const basePath = `/examples/${exampleId}/build-web`;

  window.Module = {
    canvas,
    print: (msg) => log(msg),
    printErr: (err) => log(`<span style="color:red">${err}</span>`),
    setStatus: (t) => console.log("[WASM]", t),
    locateFile: (path) => `${basePath}/${path}`,
  };

  await new Promise((resolve, reject) => {
    const script = document.createElement("script");
    script.src = `${basePath}/index.js`;
    script.async = false;

    script.onload = resolve;
    script.onerror = reject;

    document.body.appendChild(script);
  });

  moduleInstance.value = window.Module;
};

const log = (msg) => {
  if (!consoleOutput.value) return;
  console.log(msg);

  consoleOutput.value.innerHTML += msg + "<br>";
  consoleOutput.value.scrollTop = consoleOutput.value.scrollHeight;
};

onMounted(() => {
  requestAnimationFrame(async () => {
    if (!canvasRef.value) return;

    canvasRef.value.width = manifest.value?.width || 800;
    canvasRef.value.height = manifest.value?.height || 600;

    await loadModule();
  });
});

const exampleTitle = computed(() => {
  return (
    manifest.value?.name ||
    exampleId.replace(/_/g, " ").replace(/\b\w/g, (l) => l.toUpperCase())
  );
});
</script>

<template>
  <UContainer class="flex flex-col gap-4 h-full">
    <UPageHeader :title="manifest?.title || exampleTitle">
      <template #headline>
        <UBreadcrumb
          :items="[
            { label: 'Examples', to: '/examples' },
            { label: exampleTitle },
          ]"
        />
      </template>
    </UPageHeader>

    <UPageBody class="flex-1 min-h-0 flex flex-col">
      <div class="flex flex-col lg:flex-row gap-6 shrink-0">
        <div class="lg:w-2/3">
          <div
            class="bg-transparent rounded-lg flex items-center justify-center"
          >
            <canvas
              ref="canvasRef"
              id="canvas"
              :width="manifest.width"
              :height="manifest.height"
              @contextmenu.prevent
            />
          </div>
        </div>

        <div class="lg:w-1/3">
          <UCard>
            <div class="flex flex-col gap-4">
              <div class="flex flex-row gap-3">
                <UBadge v-for="tag in manifest.tags" :key="tag">
                  {{ tag }}
                </UBadge>
              </div>

              <USeparator />

              {{ manifest?.description }}

              <USeparator />

              <UDrawer
                :dismissible="false"
                title="Source Code"
                direction="right"
                :handle="false"
                v-model:open="open"
                :ui="{
                  body: 'flex flex-col h-full overflow-hidden',
                }"
              >
                <UButton
                  label="Source Code"
                  icon="i-lucide-code"
                  variant="subtle"
                  color="neutral"
                />

                <template #header>
                  <div class="flex items-center justify-between w-full">
                    <span class="font-semibold">Source Code</span>

                    <UButton
                      color="neutral"
                      variant="ghost"
                      icon="i-lucide-x"
                      @click="open = false"
                    />
                  </div>
                </template>

                <template #body>
                  <div class="flex flex-col h-full min-h-0">
                    <div class="shrink-0">
                      <UTabs
                        v-model="activeTab"
                        :items="fileTabItems"
                        variant="link"
                      />
                    </div>

                    <div class="flex-1 min-h-0 overflow-auto p-4 code-padding">
                      <div class="code text-sm" v-html="highlightedCode" />
                    </div>
                  </div>
                </template>
              </UDrawer>
            </div>
          </UCard>
        </div>
      </div>
    </UPageBody>

    <UPageCard title="Console Output" class="flex-1 min-h-0 flex flex-col">
      <div class="console-output flex-1 overflow-auto p-3 font-mono text-sm bg-black text-green-300 rounded" ref="consoleOutput" />
    </UPageCard>
  </UContainer>
</template>

<style>
.console-output {

}

.code {
  user-select: text !important;
  pointer-events: all !important;
  counter-reset: step;
  counter-increment: step 0;
}

.code .line::before {
  content: counter(step);
  counter-increment: step;
  width: 1rem;
  margin-right: 1.5rem;
  display: inline-block;
  text-align: right;
  color: rgba(115, 138, 148, 0.4);
}

.code-padding {
  background-color: rgb(30, 30, 30);
  border-radius: 2px;
}
</style>
