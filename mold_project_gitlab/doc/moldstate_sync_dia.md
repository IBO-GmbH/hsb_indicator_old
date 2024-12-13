```mermaid
sequenceDiagram
  participant box
  participant backend
  Note left of box: we are in infinit at beginning
  Note left of box: change to 4 days. timestamps for all isoplets except iso2 get set to the pseudo timestamp 1001
  box-->backend: sending {4days, isoplet_time:1001, change_time:1001}
  Note left of box: change to 2 days. timestamps for isoplet 2 get set to the pseudo timestamp 1002
  box-->backend: sending {2days, isoplet_time:1002, change_time:1002}
  Note left of box: change to 4 days again. timestamps for isoplet 2 gets invalidated.
  box-->backend: sending {4days, isoplet_time:1001, change_time:1003}
```
