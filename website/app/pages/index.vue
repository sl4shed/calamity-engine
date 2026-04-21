<script setup>
const items = [
  {label: 'Sprite', icon: 'i-lucide-image', slot: 'sprite'},
  {label: 'Input', icon: 'i-lucide-gamepad-directional', slot: 'input'},
  {label: 'Physics', icon: 'i-lucide-atom', slot: 'physics'}
]
</script>
<template>
  <div>
    <UPageHero :links="[{ label: 'Get Started', to: 'https://calamity.sl4shed.xyz/docs/getting_started.html', trailingIcon: 'i-lucide-book', size: 'xl' },
                        { label: 'Examples', to: 'examples', icon: 'i-lucide-code', size: 'xl', color: 'neutral', variant: 'subtle' }]"
               description="A small, portable game engine written in C++ with SDL3."
               title="Calamity Engine"/>

    <UPageSection description="Built with speed and fast development times in mind."
                  title="Portable game development has never been this easy"
                  headline="Features"
                  :features="[
                      {
                        title: 'API',
                        icon: 'i-lucide-code',
                        description: 'The user-facing API of Calamity Engine was built with usability in mind.'
                      },
                      {
                        title: 'Modern game engine features',
                        icon: 'i-lucide-list',
                        description: 'Every engine service was made to match the API\'s of Unity and Godot.'
                      },
                      {
                        title: 'Portability',
                        icon: 'i-lucide-plane',
                        description: 'Calamity Engine games compile to every platform that SDL3 compiles to.'
                      }
                  ]"
    />

    <UPageSection>
      <UPageCTA title="Ready to start using Calamity Engine?"
                description="I wrote enough documentation for two lifetimes, and plenty of examples."
                variant="subtle"
                :links="[{ label: 'Documentation', to: 'https://calamity.sl4shed.xyz/docs', trailingIcon: 'i-lucide-book', size: 'xl' },
                        { label: 'Examples', to: 'examples', icon: 'i-lucide-code', size: 'xl', color: 'neutral', variant: 'subtle' }]"

      />
    </UPageSection>
  </div>
</template>