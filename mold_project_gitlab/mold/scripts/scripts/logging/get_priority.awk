{
  num_fields = NF
  if (num_fields > 4)
    num_fields = 4

  for (i = 1; i <= num_fields; i++) {
    field=tolower($i)
    if (field == "warn" || field == "warning")
      prio="warning"
    else if (field == "error" || field == "err")
      prio="err"
    else if (field == "verbose" || field == "debug")
      prio="debug"
    else if (field == "info" || field == "information")
      prio="info"
    else if (field == "crit" || field == "critical")
      prio="crit"
  }
  print prio
}
