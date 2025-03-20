#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H
#define TEXT_VARIANTS 13

#include <stdio.h>
typedef struct {
    const char *id;
    const char *msgs[TEXT_VARIANTS];
} OS_Funny;


static OS_Funny os_funnys[] = {
    { "nixos", {
        "NixOS, modular marvel",
        "NixOS, uniquely stable",
        "NixOS, reimagined",
        "NixOS, functional fortress",
        "NixOS, configuration champion",
        "NixOS, reproducible wonder",
        "NixOS, modular and mighty",
        "NixOS, the declarative dream",
        "NixOS, system integrity at its best",
        "NixOS, state-of-the-art simplicity",
        "NixOS, a new era of Linux",
        "NixOS, pure and dependable",
        "NixOS, where configuration meets innovation"
    } },
    { "arch", {
        "Arch Linux, rolling perfection",
        "Arch Linux, pure simplicity",
        "Arch Linux, cutting edge",
        "Arch Linux, minimal and powerful",
        "Arch Linux, custom-built brilliance",
        "Arch Linux, streamlined freedom",
        "Arch Linux, the DIY dream",
        "Arch Linux, no-nonsense minimalism",
        "Arch Linux, elegance in simplicity",
        "Arch Linux, bleeding edge technology",
        "Arch Linux, reinventing minimalism",
        "Arch Linux, uncompromised design",
        "Arch Linux, pure rolling innovation"
    } },
    { "ubuntu", {
        "Ubuntu, friendly and robust",
        "Ubuntu, community built",
        "Ubuntu, reliable and smooth",
        "Ubuntu, polished and popular",
        "Ubuntu, designed for everyone",
        "Ubuntu, a warm welcome",
        "Ubuntu, out-of-the-box ease",
        "Ubuntu, solid and supportive",
        "Ubuntu, innovation with stability",
        "Ubuntu, where ease meets power",
        "Ubuntu, accessible and refined",
        "Ubuntu, a proven classic",
        "Ubuntu, consistently dependable"
    } },
    { "debian", {
        "Debian, rock solid",
        "Debian, universal and stable",
        "Debian, trusted by many",
        "Debian, the backbone of Linux",
        "Debian, stability first",
        "Debian, long-term reliability",
        "Debian, timeless and true",
        "Debian, the stalwart choice",
        "Debian, robust and refined",
        "Debian, unyielding reliability",
        "Debian, a benchmark in stability",
        "Debian, foundational and firm",
        "Debian, consistently dependable"
    } },
    { "fedora", {
        "Fedora, innovative and fresh",
        "Fedora, built for advancement",
        "Fedora, always ahead",
        "Fedora, the trailblazer",
        "Fedora, pioneering progress",
        "Fedora, where innovation meets style",
        "Fedora, power in simplicity",
        "Fedora, the modern distro",
        "Fedora, technology in motion",
        "Fedora, cutting-edge culture",
        "Fedora, bold and forward",
        "Fedora, the accelerator",
        "Fedora, designed for tomorrow"
    } },
    { "manjaro", {
        "Manjaro, easy rolling release",
        "Manjaro, modern and agile",
        "Manjaro, a refined Arch",
        "Manjaro, user-friendly innovation",
        "Manjaro, sleek and smooth",
        "Manjaro, rolling without friction",
        "Manjaro, blending power and ease",
        "Manjaro, accessible Arch",
        "Manjaro, minimalist yet complete",
        "Manjaro, refined performance",
        "Manjaro, elegant simplicity",
        "Manjaro, contemporary and capable",
        "Manjaro, where Arch meets ease"
    } },
    { "gentoo", {
        "Gentoo, compiled for performance",
        "Gentoo, optimized to the core",
        "Gentoo, built from source",
        "Gentoo, custom-tailored excellence",
        "Gentoo, performance personified",
        "Gentoo, for the discerning user",
        "Gentoo, where every compile counts",
        "Gentoo, pure source power",
        "Gentoo, refined through compilation",
        "Gentoo, detailed and deliberate",
        "Gentoo, crafted for efficiency",
        "Gentoo, built to order",
        "Gentoo, an artisan's OS"
    } },
    { "opensuse", {
        "openSUSE, a leap of stability",
        "openSUSE, versatile and reliable",
        "openSUSE, enterprise ready",
        "openSUSE, flexible innovation",
        "openSUSE, the open choice",
        "openSUSE, power in collaboration",
        "openSUSE, robust and adaptable",
        "openSUSE, evolving with purpose",
        "openSUSE, forward and flexible",
        "openSUSE, the community cornerstone",
        "openSUSE, always evolving",
        "openSUSE, the smart enterprise",
        "openSUSE, a stable leap forward"
    } },
    { "alpine", {
        "Alpine, minimal yet mighty",
        "Alpine, light and secure",
        "Alpine, streamlined efficiency",
        "Alpine, compact and powerful",
        "Alpine, simplicity in strength",
        "Alpine, built for speed",
        "Alpine, lean and optimized",
        "Alpine, minimalist mastery",
        "Alpine, secure and swift",
        "Alpine, efficiency embodied",
        "Alpine, small but effective",
        "Alpine, the tiny titan",
        "Alpine, purposefully minimal"
    } },
    { "void", {
        "Void, lean and independent",
        "Void, systemd-free and nimble",
        "Void, for the DIY enthusiast",
        "Void, uncompromised minimalism",
        "Void, purity in simplicity",
        "Void, a haven for purists",
        "Void, where minimalism reigns",
        "Void, freedom in small packages",
        "Void, defined by minimalism",
        "Void, just the essentials",
        "Void, stripped down and efficient",
        "Void, your minimal playground",
        "Void, crafted for independence"
    } },
    { "pop", {
        "Pop!_OS, designed for creators",
        "Pop!_OS, performance driven",
        "Pop!_OS, modern and innovative",
        "Pop!_OS, engineered for productivity",
        "Pop!_OS, a creative powerhouse",
        "Pop!_OS, sleek and efficient",
        "Pop!_OS, built for excellence",
        "Pop!_OS, the modern choice",
        "Pop!_OS, refined for performance",
        "Pop!_OS, crafted for creators",
        "Pop!_OS, forward-thinking design",
        "Pop!_OS, the ultimate workstation",
        "Pop!_OS, innovation in motion"
    } },
    { "linuxmint", {
        "Linux Mint, smooth and elegant",
        "Linux Mint, user friendly",
        "Linux Mint, refined and reliable",
        "Linux Mint, a breeze to use",
        "Linux Mint, classic and polished",
        "Linux Mint, simplicity perfected",
        "Linux Mint, where ease meets style",
        "Linux Mint, steady and sure",
        "Linux Mint, comfort in computing",
        "Linux Mint, a familiar friend",
        "Linux Mint, elegantly straightforward",
        "Linux Mint, designed for delight",
        "Linux Mint, reliability redefined"
    } },
    { "endeavouros", {
        "EndeavourOS, adventurous and fresh",
        "EndeavourOS, simple yet powerful",
        "EndeavourOS, a new adventure",
        "EndeavourOS, driven by community",
        "EndeavourOS, where bold meets simple",
        "EndeavourOS, pushing boundaries",
        "EndeavourOS, exploration reimagined",
        "EndeavourOS, the spirit of adventure",
        "EndeavourOS, fresh and fearless",
        "EndeavourOS, for the bold",
        "EndeavourOS, uncompromising and clear",
        "EndeavourOS, simplicity with purpose",
        "EndeavourOS, where the journey begins"
    } },
    { "clear-linux-os", {
        "Clear Linux, optimized to perform",
        "Clear Linux, fast and efficient",
        "Clear Linux, engineered for speed",
        "Clear Linux, built for high performance",
        "Clear Linux, performance perfected",
        "Clear Linux, streamlined and sharp",
        "Clear Linux, efficiency personified",
        "Clear Linux, a model of optimization",
        "Clear Linux, designed for power",
        "Clear Linux, speed in every byte",
        "Clear Linux, a performance benchmark",
        "Clear Linux, engineered for the future",
        "Clear Linux, clarity in performance"
    } },
    { "kali", {
        "Kali, for security enthusiasts",
        "Kali, the pentester's tool",
        "Kali, built for offense",
        "Kali, where security meets power",
        "Kali, the testing ground",
        "Kali, designed for intrusion",
        "Kali, a tool for the serious",
        "Kali, unmatched in penetration",
        "Kali, refined for security",
        "Kali, the ethical edge",
        "Kali, rigor in testing",
        "Kali, engineered for exploits",
        "Kali, the choice of professionals"
    } },
    { "slackware", {
        "Slackware, the original",
        "Slackware, old school reliability",
        "Slackware, classic Unix",
        "Slackware, tradition meets function",
        "Slackware, uncompromised heritage",
        "Slackware, pure and timeless",
        "Slackware, the pioneer distro",
        "Slackware, rugged and real",
        "Slackware, classic by design",
        "Slackware, no-frills computing",
        "Slackware, a blast from the past",
        "Slackware, built on principles",
        "Slackware, the enduring classic"
    } },
    { "rocky", {
        "Rocky, community-driven",
        "Rocky, solid as a rock",
        "Rocky, built for stability",
        "Rocky, reliable by design",
        "Rocky, the steadfast choice",
        "Rocky, strength in community",
        "Rocky, stability redefined",
        "Rocky, unwavering and true",
        "Rocky, a pillar of reliability",
        "Rocky, robust and resolute",
        "Rocky, the foundation of trust",
        "Rocky, built on rock-solid values",
        "Rocky, enduring in every way"
    } },
    { "alma", {
        "AlmaLinux, enterprise refined",
        "AlmaLinux, open and robust",
        "AlmaLinux, built for servers",
        "AlmaLinux, the community enterprise",
        "AlmaLinux, reliable and strong",
        "AlmaLinux, designed for data centers",
        "AlmaLinux, enterprise with heart",
        "AlmaLinux, robust by nature",
        "AlmaLinux, where business meets Linux",
        "AlmaLinux, powerful and open",
        "AlmaLinux, dependable for enterprise",
        "AlmaLinux, built to last",
        "AlmaLinux, the enterprise standard"
    } },
    { "elementary", {
        "elementary, beautiful and simple",
        "elementary, user-centric design",
        "elementary, a graceful OS",
        "elementary, elegance in every byte",
        "elementary, refined for usability",
        "elementary, minimal and charming",
        "elementary, where beauty meets function",
        "elementary, simple yet sophisticated",
        "elementary, designed with care",
        "elementary, the art of simplicity",
        "elementary, pure design",
        "elementary, a harmonious system",
        "elementary, understated elegance"
    } },
    { "garuda", {
        "Garuda, performance unleashed",
        "Garuda, dynamic and bold",
        "Garuda, spirited and agile",
        "Garuda, where power meets passion",
        "Garuda, engineered for speed",
        "Garuda, fast and fearless",
        "Garuda, performance reimagined",
        "Garuda, optimized for gamers",
        "Garuda, agile and uncompromising",
        "Garuda, bold by design",
        "Garuda, the drive for performance",
        "Garuda, speed with style",
        "Garuda, where performance is art"
    } }
};


#define OS_FUNNY_COUNT (sizeof(os_funnys) / sizeof(os_funnys[0]))


#define BUFFER_SIZE 1024
#define MAX_ALIASES 100
#define MAX_HISTORY 100


void load_config(void);
void save_config(void);
char *get_prompt_config(void);

#endif
