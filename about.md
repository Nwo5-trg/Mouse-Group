# Mouse Group
utilities 4r fw the mouse in lvls

## Stuffs
- have a group follow the mouse
- save mouse pos in item ids
- force show mouse

## Config
everything in the mod is configured w/ a *special txt obj* that starts w/ `MGC!`

now to configure the groups and items we use 4r stuffs the syntax is `[ARG][SEPERATOR][VAL]` (and js `[ARG]` 4r bools)

all valid args r

- `FG` (sets gid that follows mouse, by default takes the center of all objs in the group and folllows around that but if a group parent is specified that gets used | takes number)
- `IF` (sets item id that controls if prev mentioned gid follows mouse, if this item id is set to 0 then the gid wont follow mouse | takes number)
- `XI/YI` (sets item id that gets set to ***screen*** mouse x/y | takes number)
- `XGI/YGI` (sets item id that gets set to ***game*** (wat the gid thats following the mouse gets moved to) mouse x/y | takes number)
- `FMV` (forces the mouse to always b visible | takes nothing)

everything that isnt the arg + char and its following val is whitespace so ur txt could b `MGC!FG[6IF[2FMV` or have pretty formatting like `MGC! owo FG=5; IF=2; FMV; >w<`

## Credits
- ***NicksNews*** 4r the mod suggestion :3

## Rawr
first time making a logo in photopea hope it turned out just beautiful as always :3

// FOR TRIGGER INDICATORS

MAKE EXTRAS APPEAR FOR ALL TRIGGERS
MAKE DRAG TRIGGERS DISABLE TOUCH TRIGGERED