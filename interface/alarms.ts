const data = {
  alarms: [
    {
      id: crypto.randomUUID(),
      time: {
        hour: 7,
        minute: 30,
      },
      days: {
        sunday: false,
        monday: true,
        tuesday: true,
        wednesday: true,
        thursday: true,
        friday: true,
        saturday: false,
      },
      enabled: true,
    },
    {
      id: crypto.randomUUID(),
      time: {
        hour: 8,
        minute: 30,
      },
      days: {
        sunday: false,
        monday: true,
        tuesday: true,
        wednesday: false,
        thursday: true,
        friday: true,
        saturday: false,
      },
      enabled: false,
    },
    {
      id: crypto.randomUUID(),
      time: {
        hour: 7,
        minute: 30,
      },
      days: {
        sunday: false,
        monday: true,
        tuesday: true,
        wednesday: true,
        thursday: true,
        friday: true,
        saturday: false,
      },
      enabled: false,
    },
    {
      id: crypto.randomUUID(),
      time: {
        hour: 7,
        minute: 30,
      },
      days: {
        sunday: false,
        monday: true,
        tuesday: true,
        wednesday: true,
        thursday: true,
        friday: true,
        saturday: false,
      },
      enabled: true,
    },
  ],
};

console.log("Saving alarms...");

fetch("https://sunrise.evailson.dev/api/alarms", {
  method: "POST",
  headers: {
    "Content-Type": "application/json",
  },
  body: JSON.stringify(data),
})
  .then((res) => res.json())
  .then((data) => {
    console.log(data);
  });
