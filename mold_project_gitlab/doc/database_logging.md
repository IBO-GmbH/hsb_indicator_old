# Database logging

open the class `wolf::database`. After `m_session.open` add 

```cpp
m_session.set_log_stream(&std::cout); // std::err or std::clog would be possible too.
```

it will print the sql statements.