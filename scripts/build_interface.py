import os

Import("env")

def build_web_interface():
  os.chdir('interface')
  print("Building interface with yarn")

  try:
    env.Execute('yarn install')
    env.Execute('yarn build')
    
  except:
    print("Error building interface")
    exit(1)

  finally:
    os.chdir('..')

build_web_interface()
