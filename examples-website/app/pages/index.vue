<template>
  <div>
    <UPageHero
      title="Calamity Engine"
      description="A small, portable game engine written in C++ with SDL3."
      :links="[{
        label: 'Documenetation',
        to: 'https://calamity.sl4shed.xyz/',
        target: '_blank',
        trailingIcon: 'i-lucide-book',
        size: 'xl'
      }, {
        label: 'Examples',
        to: 'examples',
        target: '_blank',
        icon: 'i-lucide-code',
        size: 'xl',
        color: 'neutral',
        variant: 'subtle'
      }]"
    />
  </div>
</template>
