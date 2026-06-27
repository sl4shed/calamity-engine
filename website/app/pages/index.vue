<script setup>
const items = [
  { label: "Sprite", icon: "i-lucide-image", slot: "sprite" },
  { label: "Input", icon: "i-lucide-gamepad-directional", slot: "input" },
  { label: "Physics", icon: "i-lucide-atom", slot: "physics" },
];

const colorMode = useColorMode();
</script>
<template>
  <div>
    <UPageHero
      :links="[
        {
          label: 'Get Started',
          to: '/html/getting_started.html',
          target: '_blank',
          trailingIcon: 'i-lucide-book',
          size: 'xl',
        },
        {
          label: 'Examples',
          to: 'examples',
          icon: 'i-lucide-code',
          size: 'xl',
          color: 'neutral',
          variant: 'subtle',
        },
      ]"
      description="A small, portable game engine written in C++ with SDL3."
    >
      <template #title>
        <img v-if="colorMode.value === 'dark'" src="../assets/tall-full-logo.png" alt="Calamity Engine" class="h-100 w-auto mx-auto" />
        <img v-else src="../assets/tall-full-logo-light.png" alt="Calamity Engine" class="h-100 w-auto mx-auto" />
      </template>
    </UPageHero>

    <UPageSection
      description="Built with speed and fast development times in mind."
      title="Portable game development has never been this easy"
      headline="Features"
      :features="[
        {
          title: 'API',
          icon: 'i-lucide-code',
          description:
            'The user-facing API of Calamity Engine was built with usability in mind.',
        },
        {
          title: 'Modern game engine features',
          icon: 'i-lucide-list',
          description:
            'Every engine service was made to match the API\'s of Unity and Godot.',
        },
        {
          title: 'Portability',
          icon: 'i-lucide-plane',
          description:
            'Calamity Engine games compile to every platform that SDL3 compiles to.',
        },
      ]"
    />

    <UPageSection>
      <UPageCTA
        title="Ready to start using Calamity Engine?"
        description="I wrote some pretty cool documentation and examples. so maybe you should"
        variant="subtle"
        :links="[
          {
            label: 'Documentation',
            to: '/html/index.html',
            target: '_blank',
            trailingIcon: 'i-lucide-book',
            size: 'xl',
          },
          {
            label: 'Examples',
            to: 'examples',
            icon: 'i-lucide-code',
            size: 'xl',
            color: 'neutral',
            variant: 'subtle',
          },
        ]"
      />
    </UPageSection>
  </div>
</template>
